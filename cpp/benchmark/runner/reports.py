"""
Academic executive summary and offline interactive dashboard report generation
for the dtsort benchmark suite.
"""

import json
import sqlite3
import ssl
import sys
import urllib.request
from pathlib import Path
from typing import NamedTuple

import cpuinfo

from runner.db import db_query, db_query_one


class SorterCharacteristic(NamedTuple):
    name: str
    filename: str
    mode: str
    leaves: int | None
    nodes: int | None
    header_size_kb: float


def get_cpu_info() -> str:
    """
    Queries standard host CPU brand name string.
    """
    try:
        info = cpuinfo.get_cpu_info()
        cpu_name = info.get("brand_raw")
        if cpu_name:
            return cpu_name
    except Exception:
        pass
    return "Unknown CPU"


def get_chartjs_code(bench_dir: Path) -> str:
    """
    Retrieves and caches Chart.js code to support offline interactive dashboards.

    Attempts to read from templates directory first; falls back to jsDelivr CDN download.
    """
    cache_file = bench_dir / "templates" / "chart.umd.js"
    if cache_file.exists():
        return cache_file.read_text(encoding="utf-8")

    url = "https://cdn.jsdelivr.net/npm/chart.js@4.4.2/dist/chart.umd.js"
    print(f"Downloading Chart.js from {url} for inlining...", flush=True)
    try:
        ctx = ssl.create_default_context()
        with urllib.request.urlopen(url, context=ctx, timeout=15) as response:
            code = response.read().decode("utf-8")
            cache_file.write_text(code, encoding="utf-8")
            return code
    except Exception as e:
        print(
            f"Warning: Could not download Chart.js ({e}). Offline charting is unavailable.", file=sys.stderr, flush=True
        )
        return "/* Chart.js download failed. Offline charting unavailable unless cached. */"


def generate_executive_summary(
    conn: sqlite3.Connection, run_id: int, active_headers: dict[int, SorterCharacteristic], report_dir: Path
) -> None:
    """
    Generates a condensed markdown executive summary report.
    """
    run_info = db_query_one(conn, "SELECT * FROM run_info WHERE id = ?", (run_id,))

    rows = db_query(conn, "SELECT n, object_bytes FROM code_size WHERE run_id = ?", (run_id,))
    obj_sizes = {r["n"]: r["object_bytes"] for r in rows}

    pure_rows = db_query(
        conn,
        "SELECT n, max_cmp, avg_cmp, avg_move, max_move FROM consistency WHERE run_id = ? AND impl = 'PURE_GS'",
        (run_id,),
    )
    pure_metrics = {r["n"]: r for r in pure_rows}

    summary_path = report_dir / "executive_summary.md"

    timestamp_val = run_info["timestamp"] if run_info else "N/A"
    git_hash_val = run_info["git_hash"] if run_info else "N/A"
    lines = [
        "# dtsort Sorter Performance: Executive Summary",
        "",
        f"- **Timestamp**: `{timestamp_val}`",
        f"- **Git Commit**: `{git_hash_val}`",
    ]
    max_n = run_info["max_n"] if run_info else 10
    block_size = run_info["dt_block_size"] if run_info and run_info["dt_block_size"] else (16 * max_n)
    mode_val = run_info["mode"] if run_info else "N/A"
    lines.extend(
        [
            f"- **Mode**: `{mode_val}` (Block Size: `{block_size}`, sizeof(T) > 32 Threshold)",
            "",
            "## 1. Peak Speedup Cheat Sheet",
            "",
            "The table below displays the maximum speedup gains achieved by `dt_stable_sort` (High Entropy / Adaptive Mode) relative to standard `std::stable_sort` on large arrays ($N \\ge 30$).",
            "",
            "| Type | Distribution | Peak Speedup (High Entropy) | Optimal Size (N) | Peak Speedup (Adaptive Mode) | Optimal Size (N) |",
            "| :--- | :--- | :--- | :--- | :--- | :--- |",
        ]
    )

    types = ["IntNaked", "WideRow"]
    patterns = ["random", "sorted", "reverse"]
    pattern_labels = {"random": "Random", "sorted": "Sorted", "reverse": "Reversed"}

    for t in types:
        for p in patterns:
            em_r = db_query_one(
                conn,
                """
                SELECT s.size, (s.real_time_ns / d.real_time_ns) as speedup
                FROM wall_clock s
                JOIN wall_clock d ON s.run_id = d.run_id AND s.impl = d.impl AND s.type = d.type AND s.pattern = d.pattern AND s.size = d.size
                WHERE s.run_id = ? AND s.impl = 'dt' AND s.type = ? AND s.pattern = ?
                AND s.variant = 'stock' AND d.variant = 'dtsort-em' AND s.size >= 30
                ORDER BY speedup DESC LIMIT 1
                """,
                (run_id, t, p),
            )

            am_r = db_query_one(
                conn,
                """
                SELECT s.size, (s.real_time_ns / d.real_time_ns) as speedup
                FROM wall_clock s
                JOIN wall_clock d ON s.run_id = d.run_id AND s.impl = d.impl AND s.type = d.type AND s.pattern = d.pattern AND s.size = d.size
                WHERE s.run_id = ? AND s.impl = 'dt' AND s.type = ? AND s.pattern = ?
                AND s.variant = 'stock' AND d.variant = 'dtsort-am' AND s.size >= 30
                ORDER BY speedup DESC LIMIT 1
                """,
                (run_id, t, p),
            )

            em_str = (
                f"+{((em_r['speedup'] - 1.0) * 100):.1f}%"
                if em_r and em_r["speedup"] > 1.0
                else f"-{((1.0 - em_r['speedup']) * 100):.1f}%"
                if em_r
                else "—"
            )
            em_sz = f"{em_r['size']:,}" if em_r else "—"
            am_str = (
                f"+{((am_r['speedup'] - 1.0) * 100):.1f}%"
                if am_r and am_r["speedup"] > 1.0
                else f"-{((1.0 - am_r['speedup']) * 100):.1f}%"
                if am_r
                else "—"
            )
            am_sz = f"{am_r['size']:,}" if am_r else "—"

            lines.append(f"| **{t}** | {pattern_labels[p]} | **{em_str}** | {em_sz} | **{am_str}** | {am_sz} |")

    lines.extend(
        [
            "",
            "> **Note on Move Savings**: The reported $1.0N$ moves are bounded and occur specifically within the cache-aligned group index sorting phase for heavy objects (`sizeof(T) > 32` bytes) within each block of size $M = 160$ elements.",
            "",
            "## 2. Sorter Characteristics Table",
            "",
            "Complete theoretical and code size breakdown for each generated decision tree module ($N = 2..10$).",
            "",
            "| Sorter | Mode / Split | Worst-Case Cmp | Avg Cmp | Avg Move | Max Move | Header Size (KB) | Object Size (KB) |",
            "| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: |",
        ]
    )

    for sz in sorted(active_headers.keys()):
        h = active_headers[sz]
        pm = pure_metrics.get(sz)
        obj_sz = obj_sizes.get(sz, 0) / 1024.0

        worst_cmp = pm["max_cmp"] if pm and pm["max_cmp"] is not None else (h.nodes // 2 if h.nodes else "—")
        avg_cmp = f"{pm['avg_cmp']:.2f}" if pm and pm["avg_cmp"] is not None else "—"
        avg_move = f"{pm['avg_move']:.2f}" if pm and pm["avg_move"] is not None else "—"
        worst_move = pm["max_move"] if pm and pm["max_move"] is not None else "—"

        lines.append(
            f"| `{h.name}` | `{h.mode}` | {worst_cmp} | {avg_cmp} | {avg_move} | {worst_move} | "
            f"{h.header_size_kb:.2f} KB | {obj_sz:.2f} KB |"
        )

    summary_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"Generated executive summary: {summary_path.name}", flush=True)


def generate_interactive_dashboard(
    conn: sqlite3.Connection,
    run_id: int,
    active_headers: dict[int, SorterCharacteristic],
    report_dir: Path,
    bench_dir: Path,
) -> None:
    """
    Generates a unified offline HTML interactive dashboard inlined with Chart.js.
    """
    run_info = db_query_one(conn, "SELECT * FROM run_info WHERE id = ?", (run_id,))

    rows = db_query(
        conn,
        """
        SELECT impl, variant, type, pattern, size, AVG(real_time_ns) as real_ns
        FROM wall_clock
        WHERE run_id = ?
        GROUP BY impl, variant, type, pattern, size
        ORDER BY size
        """,
        (run_id,),
    )
    wall_clock_records = [dict(r) for r in rows]

    size_rows = db_query(conn, "SELECT n, object_bytes FROM code_size WHERE run_id = ?", (run_id,))
    obj_sizes = {r["n"]: r["object_bytes"] for r in size_rows}

    pure_rows = db_query(
        conn,
        "SELECT n, max_cmp, avg_cmp, avg_move, max_move FROM consistency WHERE run_id = ? AND impl = 'PURE_GS'",
        (run_id,),
    )
    pure_metrics = {r["n"]: r for r in pure_rows}

    sorter_records = []
    for sz in sorted(active_headers.keys()):
        h = active_headers[sz]
        pm = pure_metrics.get(sz)
        worst_cmp = pm["max_cmp"] if pm else 0
        avg_cmp = pm["avg_cmp"] if pm else 0.0
        avg_move = pm["avg_move"] if pm else 0.0
        max_move = pm["max_move"] if pm else 0

        sorter_records.append(
            {
                "size": sz,
                "name": h.name,
                "mode": h.mode,
                "leaves": h.leaves or worst_cmp or 0,
                "nodes": h.nodes or (worst_cmp * 2 if worst_cmp else 0),
                "worst_cmp": worst_cmp,
                "avg_cmp": avg_cmp,
                "avg_move": avg_move,
                "max_move": max_move,
                "header_size_kb": h.header_size_kb,
                "object_size_kb": obj_sizes.get(sz, 0) / 1024.0,
            }
        )

    chartjs_code = get_chartjs_code(bench_dir)

    template_path = bench_dir / "templates" / "dashboard_template.html"
    if not template_path.exists():
        print(f"Error: Dashboard template not found at {template_path}", file=sys.stderr, flush=True)
        return

    html_template = template_path.read_text(encoding="utf-8")

    git_hash_val = run_info["git_hash"] if run_info else "N/A"
    html_content = html_template.replace("{{GIT_HASH}}", git_hash_val)
    html_content = html_content.replace("{{TARGET_PLATFORM}}", get_cpu_info())

    max_n = run_info["max_n"] if run_info else 10
    block_size = run_info["dt_block_size"] if run_info and run_info["dt_block_size"] else (16 * max_n)
    html_content = html_content.replace("{{BLOCK_SIZE}}", str(block_size))

    timestamp_val = run_info["timestamp"] if run_info else "N/A"
    run_date = timestamp_val.replace("T", " ")
    html_content = html_content.replace("{{RUN_DATE}}", run_date)
    html_content = html_content.replace("{{CHARTJS_CODE}}", chartjs_code)

    benchmark_data_json = json.dumps({"wall_clock": wall_clock_records, "sorters": sorter_records})
    html_content = html_content.replace("{{BENCHMARK_DATA_JSON}}", benchmark_data_json)

    html_path = report_dir / "interactive_report.html"
    html_path.write_text(html_content, encoding="utf-8")
    print(f"Generated interactive dashboard: {html_path.name}", flush=True)
