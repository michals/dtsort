"""
SQLite database analysis and summary report generation for the dtsort benchmark suite.
"""

import sqlite3
import sys
from collections import defaultdict
from pathlib import Path

from runner.db import db_query, db_query_one


def _format_gain_percentage(stock: float, dt: float) -> str:
    """
    Formatted gain string for performance improvement/degradation comparison.
    """
    if stock == 0 or dt == 0:
        return "—"
    if stock == dt:
        return "0.0%"
    gain_pct = ((stock / dt) - 1.0) * 100.0
    sign = "+" if gain_pct > 0 else ""
    text = f"{sign}{gain_pct:.1f}%"
    if gain_pct > 0.5:
        return f"**{text}**"
    return text


def run_database_analysis(db_path: Path, output_path: Path) -> bool:
    """
    Queries the SQLite results database to generate a comprehensive markdown report.
    """
    conn = sqlite3.connect(str(db_path))
    conn.row_factory = sqlite3.Row

    try:
        # Load Run Info
        run = db_query_one(conn, "SELECT * FROM run_info ORDER BY id DESC LIMIT 1")
        if not run:
            print(f"Warning: No run metadata found in {db_path}. Is the database empty?", file=sys.stderr)
            return False
        run_id = run["id"]

        lines = [
            "# Benchmark Summary: Stock vs. dtsort (DB Driven)",
            "",
            "*Note: All benchmarks invoke `target_std::stable_sort()` as the base reference algorithm.*",
            "",
            "## 1. Summary",
            "",
            f"- **Max N (K)**: {run['max_n']}",
        ]

        mode_str = f"{run['mode']}"
        if run["compiled_mode"] and run["compiled_mode"] != run["mode"]:
            mode_str += f" (Compiled: {run['compiled_mode']})"
        lines.append(f"- **Sort Mode**: {mode_str}")

        if run["dt_threshold"]:
            lines.append(f"- **Sort Threshold**: {run['dt_threshold']}")
        if run["dt_block_size"]:
            lines.append(f"- **Block Size**: {run['dt_block_size']}")
        lines.append(f"- **Git Hash**: {run['git_hash']}")
        if run["cmdline"]:
            lines.append(f"- **Command**: `{run['cmdline']}`")
        lines.append(f"- **Timestamp**: {run['timestamp']}")
        lines.append(f"- **Iterations (Counts)**: {run['count_iters']}")
        lines.append("")

        # 2. Pure dtsort theoretical bounds
        lines.extend(
            [
                "## 2. Pure dtsort",
                "",
                "These metrics are calculated by exhaustively iterating through ALL permutations.",
                "This represents the absolute theoretical limit of the dtsort algorithm for small N.",
                "",
                "*Source: `consistency` table*",
                "",
            ]
        )

        pure_impls_rows = db_query(
            conn,
            "SELECT DISTINCT impl FROM consistency WHERE run_id = ? AND impl LIKE 'PURE_%' ORDER BY impl",
            (run_id,),
        )
        pure_impls = [r["impl"] for r in pure_impls_rows]

        if pure_impls:
            ordered_pure = []
            if "PURE_GS" in pure_impls:
                ordered_pure.append("PURE_GS")
            if "PURE_FULL" in pure_impls:
                ordered_pure.append("PURE_FULL")
            if "PURE_PS" in pure_impls:
                ordered_pure.append("PURE_PS")

            for p in pure_impls:
                if p not in ordered_pure:
                    ordered_pure.append(p)

            for impl in ordered_pure:
                label = (
                    "GS"
                    if impl == "PURE_GS"
                    else "FULL"
                    if impl == "PURE_FULL"
                    else "PS"
                    if impl == "PURE_PS"
                    else impl
                )
                lines.extend(
                    [
                        f"### Mode: {label}",
                        "| N | Avg Cmp | Max Cmp | Avg Move | Max Move |",
                        "|---|---------|---------|----------|----------|",
                    ]
                )
                rows = db_query(
                    conn, "SELECT * FROM consistency WHERE run_id = ? AND impl = ? ORDER BY n", (run_id, impl)
                )
                for row in rows:
                    lines.append(
                        f"| {row['n']} | {row['avg_cmp']:.3f} | {row['max_cmp']} | "
                        f"{row['avg_move']:.3f} | {row['max_move']} |"
                    )
                lines.append("")
        else:
            lines.append("*Theoretical metrics not available.*")
            lines.append("")

        # 2.2 Code Size Impact
        lines.extend(
            [
                "### 2.2 Code Size Impact",
                "",
                "*Source: `code_size` table*",
                "",
                "| N | Header Lines | Header Size | Object Size |",
                "|---|--------------|-------------|-------------|",
            ]
        )

        rows = db_query(conn, "SELECT * FROM code_size WHERE run_id = ? ORDER BY n", (run_id,))
        for row in rows:
            h_kb = row["source_bytes"] / 1024
            o_kb = row["object_bytes"] / 1024
            lines.append(f"| {row['n']} | {row['source_lines']:,} | {h_kb:.1f} KB | {o_kb:.1f} KB |")
        lines.append("")

        # 3. Algorithmic Counts
        lines.extend(
            [
                "## 3. Algorithmic Counts",
                "",
                "Comparing total comparisons and moves between stock std::stable_sort and dtsort-patched version.",
                "",
                "### 3.1 Baseline Validation",
                "",
                "Validates that dtsort never exceeds stock comparison or move counts across all permutations.",
                "",
                "*Source: `v_consistency_summary` view*",
                "",
            ]
        )

        impls_rows = db_query(
            conn, "SELECT DISTINCT impl FROM v_consistency_summary WHERE run_id = ? ORDER BY impl", (run_id,)
        )
        impls = [r["impl"] for r in impls_rows]

        for impl in impls:
            lines.extend(
                [
                    f"#### {impl} Validation",
                    "| Variant | N | Stock Max Cmp | dtsort Max Cmp | Gain | Stock Max Move | dtsort Max Move | Gain | Valid? |",
                    "|---------|---|---------------|----------------|------|----------------|-----------------|------|--------|",
                ]
            )

            rows = db_query(
                conn, "SELECT * FROM v_consistency_summary WHERE run_id = ? AND impl = ? ORDER BY n", (run_id, impl)
            )
            for row in rows:
                stock_mc, dt_mc = row["stock_max_cmp"], row["dtsort_max_cmp"]
                stock_mm, dt_mm = row["stock_max_move"], row["dtsort_max_move"]

                if stock_mc is not None and dt_mc is not None:
                    cmp_gain_str = _format_gain_percentage(stock_mc, dt_mc)
                else:
                    cmp_gain_str = "—"

                if stock_mm is not None and dt_mm is not None:
                    move_gain_str = _format_gain_percentage(stock_mm, dt_mm)
                else:
                    move_gain_str = "—"

                valid = (
                    "✅"
                    if dt_mc is not None
                    and stock_mc is not None
                    and dt_mc <= stock_mc
                    and dt_mm is not None
                    and stock_mm is not None
                    and dt_mm <= stock_mm
                    else ("—" if dt_mc is None else "❌")
                )

                lines.append(
                    f"| {row['variant']} | {row['n']} | {stock_mc if stock_mc is not None else '—'} | "
                    f"{dt_mc if dt_mc is not None else '—'} | {cmp_gain_str} | "
                    f"{stock_mm if stock_mm is not None else '—'} | {dt_mm if dt_mm is not None else '—'} | "
                    f"{move_gain_str} | {valid} |"
                )
            lines.append("")

        lines.extend(
            [
                "### 3.2 Small N Average Counts",
                "",
                "*Source: `v_consistency_summary` view*",
                "",
                "#### Comparison Counts",
                "| Impl | Variant | N | Stock Cmp (Avg/Max) | dtsort Cmp (Avg/Max) | Gain (Avg) | Gain (Max) | Valid? |",
                "|------|---------|---|---------------------|----------------------|------------|------------|--------|",
            ]
        )

        rows = db_query(conn, "SELECT * FROM v_consistency_summary WHERE run_id = ? ORDER BY impl, n", (run_id,))
        for row in rows:
            stock_ac, dt_ac = row["stock_avg_cmp"], row["dtsort_avg_cmp"]
            stock_mc, dt_mc = row["stock_max_cmp"], row["dtsort_max_cmp"]

            if stock_ac is not None and dt_ac is not None:
                avg_gain_str = _format_gain_percentage(stock_ac, dt_ac)
            else:
                avg_gain_str = "—"

            if stock_mc is not None and dt_mc is not None:
                max_gain_str = _format_gain_percentage(stock_mc, dt_mc)
            else:
                max_gain_str = "—"

            valid = (
                "✅"
                if dt_mc is not None and stock_mc is not None and dt_mc <= stock_mc
                else ("—" if dt_mc is None else "❌")
            )

            sc_str = f"{stock_ac:.2f}/{stock_mc}" if stock_ac is not None else "—/—"
            dc_str = f"{dt_ac:.2f}/{dt_mc}" if dt_ac is not None else "—/—"

            lines.append(
                f"| {row['impl']} | {row['variant']} | {row['n']} | {sc_str} | "
                f"{dc_str} | {avg_gain_str} | {max_gain_str} | {valid} |"
            )
        lines.append("")

        lines.extend(
            [
                "#### Move Counts",
                "| Impl | Variant | N | Stock Move (Avg/Max) | dtsort Move (Avg/Max) | Gain (Avg) | Gain (Max) | Valid? |",
                "|------|---------|---|----------------------|-----------------------|------------|------------|--------|",
            ]
        )

        rows = db_query(conn, "SELECT * FROM v_consistency_summary WHERE run_id = ? ORDER BY impl, n", (run_id,))
        for row in rows:
            stock_am, dt_am = row["stock_avg_move"], row["dtsort_avg_move"]
            stock_mm, dt_mm = row["stock_max_move"], row["dtsort_max_move"]

            if stock_am is not None and dt_am is not None:
                avg_gain_str = _format_gain_percentage(stock_am, dt_am)
            else:
                avg_gain_str = "—"

            if stock_mm is not None and dt_mm is not None:
                max_gain_str = _format_gain_percentage(stock_mm, dt_mm)
            else:
                max_gain_str = "—"

            valid = (
                "✅"
                if dt_mm is not None and stock_mm is not None and dt_mm <= stock_mm
                else ("—" if dt_mm is None else "❌")
            )

            sm_str = f"{stock_am:.2f}/{stock_mm}" if stock_am is not None else "—/—"
            dm_str = f"{dt_am:.2f}/{dt_mm}" if dt_am is not None else "—/—"

            lines.append(
                f"| {row['impl']} | {row['variant']} | {row['n']} | {sm_str} | "
                f"{dm_str} | {avg_gain_str} | {max_gain_str} | {valid} |"
            )
        lines.append("")

        # 3.3 Large Dataset Metrics
        lines.extend(
            [
                "### 3.3 Large Dataset Metrics",
                "",
                "*Source: `v_large_n_summary` view*",
                "",
            ]
        )

        sizes_rows = db_query(
            conn, "SELECT DISTINCT size FROM v_large_n_summary WHERE run_id = ? ORDER BY size", (run_id,)
        )
        sizes = [r["size"] for r in sizes_rows]

        for size in sizes:
            lines.extend(
                [
                    f"#### N={size:,}",
                    "",
                    "| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |",
                    "|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|",
                ]
            )

            rows = db_query(
                conn,
                "SELECT * FROM v_large_n_summary WHERE run_id = ? AND size = ? ORDER BY impl, type",
                (run_id, size),
            )
            for row in rows:
                stock_cmp = row["stock_avg_cmp"]
                dt_cmp = row["dtsort_avg_cmp"]
                stock_move = row["stock_avg_move"]
                dt_move = row["dtsort_avg_move"]

                if stock_cmp is not None and dt_cmp is not None:
                    cmp_gain_str = _format_gain_percentage(stock_cmp, dt_cmp)
                    scmp_str, dcmp_str = f"{stock_cmp:.2f}", f"{dt_cmp:.2f}"
                else:
                    cmp_gain_str, scmp_str, dcmp_str = "—", "—", "—"

                if stock_move is not None and dt_move is not None:
                    move_gain_str = _format_gain_percentage(stock_move, dt_move)
                    smove_str, dmove_str = f"{stock_move:.2f}", f"{dt_move:.2f}"
                else:
                    move_gain_str, smove_str, dmove_str = "—", "—", "—"

                lines.append(
                    f"| {row['impl']} | {row['variant']} | {row['type']} | {scmp_str} | {dcmp_str} | {cmp_gain_str} | "
                    f"{smove_str} | {dmove_str} | {move_gain_str} |"
                )
            lines.append("")

        # 4. Wallclock timing measurements
        lines.extend(
            [
                "## 4. Wallclock Measurements",
                "",
                "### 4.1 Throughput Analysis",
                "",
                "*Source: `v_wall_clock_summary` view*",
                "",
            ]
        )

        groups = db_query(
            conn,
            "SELECT DISTINCT impl, type, pattern FROM v_wall_clock_summary WHERE run_id = ? ORDER BY impl, type, pattern",
            (run_id,),
        )
        for g in groups:
            lines.extend(
                [
                    f"#### {g['impl']} ({g['type']}{' (' + g['pattern'].capitalize() + ')' if g['pattern'] != 'random' else ''})",
                    "| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |",
                    "|------------|---------|----------|-----------|------|--------|",
                ]
            )

            rows = db_query(
                conn,
                "SELECT * FROM v_wall_clock_summary WHERE run_id = ? AND impl = ? AND type = ? AND pattern = ? ORDER BY size",
                (run_id, g["impl"], g["type"], g["pattern"]),
            )
            for row in rows:
                gain_str = _format_gain_percentage(row["stock_ns"], row["dtsort_ns"])
                gain_val = ((row["stock_ns"] / max(1.0, row["dtsort_ns"])) - 1.0) * 100.0
                status = "✅" if gain_val > -1.0 else "⚠️"
                lines.append(
                    f"| {row['size']:,} | {row['variant']} | {row['stock_ns']:.2f} | {row['dtsort_ns']:.2f} | {gain_str} | {status} |"
                )
            lines.append("")

        # 4.2 Data Type Weights
        lines.extend(
            [
                "### 4.2 Data Type Weights",
                "",
                "*Source: `v_weight_summary` view*",
                "",
                "| Data Type | Cmp (ns) | Cmp Factor | Move (ns) | Move Factor |",
                "|-----------|----------|------------|-----------|-------------|",
            ]
        )

        weights = db_query(conn, "SELECT * FROM v_weight_summary WHERE run_id = ? ORDER BY cmp_ns", (run_id,))
        if weights:
            base_cmp = weights[0]["cmp_ns"]
            base_move = weights[0]["move_ns"]
            for row in weights:
                cf = row["cmp_ns"] / base_cmp
                mf = row["move_ns"] / base_move
                lines.append(f"| {row['type']} | {row['cmp_ns']:.2f} | {cf:.1f}x | {row['move_ns']:.2f} | {mf:.1f}x |")
        lines.append("")

        # 5. Adaptivity Comparison
        lines.extend(
            [
                "## 5. Adaptivity Comparison (dt_stable_sort)",
                "",
                "Comparing the two main configuration modes: High Entropy (No early exits or merge-skipping) vs. Adaptive Mode (GS shortcuts + merge-skipping).",
                "",
                "*Source: `wall_clock` table*",
                "",
            ]
        )

        rows = db_query(
            conn,
            """
            SELECT
                type, pattern, size,
                MAX(CASE WHEN variant = 'dtsort-em' THEN real_time_ns END) as em_ns,
                MAX(CASE WHEN variant = 'dtsort-am' THEN real_time_ns END) as am_ns
            FROM wall_clock
            WHERE run_id = ? AND impl = 'dt'
            GROUP BY type, pattern, size
            ORDER BY type, pattern, size
        """,
            (run_id,),
        )

        if rows:
            grouped = defaultdict(list)
            for r in rows:
                if r["em_ns"] is not None and r["am_ns"] is not None:
                    grouped[(r["type"], r["pattern"])].append(r)

            for (dtype, pattern), r_list in sorted(grouped.items()):
                lines.extend(
                    [
                        f"#### {dtype} ({pattern.capitalize()})",
                        "| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |",
                        "|------|-------------------|-------------------|------|",
                    ]
                )
                for r in r_list:
                    em, am = r["em_ns"], r["am_ns"]
                    gain_str = _format_gain_percentage(em, am)
                    lines.append(f"| {r['size']:,} | {em:.2f} | {am:.2f} | {gain_str} |")
                lines.append("")
        else:
            lines.append("*No adaptivity results available.*")
            lines.append("")

        output_path.write_text("\n".join(lines), encoding="utf-8")
        return True

    finally:
        conn.close()
