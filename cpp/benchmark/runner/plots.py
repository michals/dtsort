"""
Academic and performance SVG figure generation for the dtsort benchmark suite.
"""

import math
import sqlite3
import sys
from pathlib import Path
from typing import Any

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

from runner.consts import OEIS_A036604
from runner.db import db_query, db_query_one


def generate_fig1_speedup(conn: sqlite3.Connection, run_id: int, report_dir: Path) -> None:
    """
    Generates Fig 1: Scale-Up Performance Curve Grid (2x3).

    Plots relative execution speedup factor against baseline stock compilers
    for IntNaked and WideRow across random, sorted, and reversed input distributions.
    """
    rows = db_query(
        conn,
        """
        SELECT size, variant, type, pattern, AVG(real_time_ns) as avg_ns
        FROM wall_clock
        WHERE run_id = ? AND impl = 'dt' AND size >= 30
        GROUP BY size, variant, type, pattern
        ORDER BY size
        """,
        (run_id,),
    )

    if not rows:
        print("Warning: No timing data found in database for Fig 1.", file=sys.stderr, flush=True)
        return

    # Group data by [type][pattern][variant] = {size: avg_ns}
    data: dict[str, dict[str, dict[str, dict[int, float]]]] = {}
    for r in rows:
        t_type = r["type"]
        t_pattern = r["pattern"]
        t_variant = r["variant"]
        t_size = r["size"]
        t_ns = r["avg_ns"]

        if t_type not in data:
            data[t_type] = {}
        if t_pattern not in data[t_type]:
            data[t_type][t_pattern] = {}
        if t_variant not in data[t_type][t_pattern]:
            data[t_type][t_pattern][t_variant] = {}
        data[t_type][t_pattern][t_variant][t_size] = t_ns

    types = ["IntNaked", "WideRow"]
    patterns = ["random", "sorted", "reverse"]
    pattern_labels = {"random": "Random", "sorted": "Sorted", "reverse": "Reversed"}

    fig, axes = plt.subplots(2, 3, figsize=(14, 8), sharex=True)

    for row_idx, t_type in enumerate(types):
        for col_idx, pattern in enumerate(patterns):
            ax = axes[row_idx, col_idx]
            ax.axhline(1.0, color="gray", linestyle="--", alpha=0.5, linewidth=1.0)

            # Fetch sizes and calculate speedups
            type_data = data.get(t_type, {}).get(pattern, {})
            stock_data = type_data.get("stock", {})

            for variant, label, color, marker in [
                ("dtsort-em", "dtsort (High Entropy)", "#1f77b4", "o"),
                ("dtsort-am", "dtsort (Adaptive Mode)", "#ff7f0e", "s"),
            ]:
                var_data = type_data.get(variant, {})
                common_sizes = sorted(list(set(stock_data.keys()) & set(var_data.keys())))

                if not common_sizes:
                    continue

                speedups = [stock_data[sz] / var_data[sz] for sz in common_sizes]

                ax.plot(
                    common_sizes,
                    speedups,
                    label=label,
                    color=color,
                    marker=marker,
                    markersize=4,
                    linewidth=1.5,
                    alpha=0.8,
                )

            ax.set_xscale("log")
            ax.grid(True, which="both", ls="-", color="0.9", alpha=0.5)

            if row_idx == 0:
                ax.set_title(f"{pattern_labels[pattern]}", fontsize=12, fontweight="bold")
            if col_idx == 0:
                ax.set_ylabel(f"{t_type}\nSpeedup Factor", fontsize=11)
            else:
                ax.set_ylabel("", fontsize=11)

            if row_idx == 1:
                ax.set_xlabel("Dataset Size (N)", fontsize=11)

            if row_idx == 0 and col_idx == 2:
                ax.legend(loc="upper left", frameon=True, shadow=False)

    fig.suptitle(
        "dtsort Scale-Up Performance (Speedup relative to Stock std::stable_sort)",
        fontsize=14,
        fontweight="bold",
        y=0.98,
    )
    plt.tight_layout()

    out_path = report_dir / "fig1_speedup_grid.svg"
    plt.savefig(out_path, format="svg", bbox_inches="tight")
    plt.close()
    print(f"Generated academic figure 1: {out_path.name}", flush=True)


def generate_fig2_throughput(conn: sqlite3.Connection, run_id: int, report_dir: Path) -> None:
    """
    Generates Fig 2: Leaf-Level Throughput Grouped Bar Chart.

    Plots total sorts per second for small N arrays (N=2..10), showing the transition
    boundary between exhaustive Full DT and gsDT split topologies.
    """
    rows = db_query(
        conn,
        """
        SELECT size, variant, AVG(real_time_ns) as avg_ns
        FROM wall_clock
        WHERE run_id = ? AND impl = 'dt' AND type = 'IntNaked' AND pattern = 'random' AND size <= 10
        GROUP BY size, variant
        ORDER BY size
        """,
        (run_id,),
    )

    if not rows:
        print("Warning: No timing data found for Fig 2.", file=sys.stderr, flush=True)
        return

    data: dict[int, dict[str, float]] = {}
    for r in rows:
        size = r["size"]
        var = r["variant"]
        ns = r["avg_ns"]
        if size not in data:
            data[size] = {}
        data[size][var] = ns

    sizes = sorted(list(data.keys()))
    if not sizes:
        return

    stock_tp = []
    em_tp = []
    am_tp = []

    for sz in sizes:
        var_data = data[sz]
        stock_tp.append(1000.0 / var_data.get("stock", 1.0) if "stock" in var_data else 0.0)
        em_tp.append(1000.0 / var_data.get("dtsort-em", 1.0) if "dtsort-em" in var_data else 0.0)
        am_tp.append(1000.0 / var_data.get("dtsort-am", 1.0) if "dtsort-am" in var_data else 0.0)

    fig, ax = plt.subplots(figsize=(10, 5))
    x = np.arange(len(sizes))
    width = 0.25

    ax.bar(
        x - width,
        stock_tp,
        width,
        label="Stock fallback",
        color="#7f7f7f",
        alpha=0.8,
        edgecolor="black",
        linewidth=0.5,
    )
    ax.bar(
        x,
        em_tp,
        width,
        label="dtsort (High Entropy)",
        color="#1f77b4",
        alpha=0.9,
        edgecolor="black",
        linewidth=0.5,
    )
    ax.bar(
        x + width,
        am_tp,
        width,
        label="dtsort (Adaptive Mode)",
        color="#ff7f0e",
        alpha=0.9,
        edgecolor="black",
        linewidth=0.5,
    )

    ax.axvline(3.5, color="red", linestyle="--", alpha=0.7, linewidth=1.5)
    ax.axvspan(-0.5, 3.5, color="blue", alpha=0.03, label="_nolegend_")
    ax.axvspan(3.5, len(sizes) - 0.5, color="green", alpha=0.03, label="_nolegend_")

    ax.text(
        1.5,
        ax.get_ylim()[1] * 0.9,
        "Full DT (N=2..5)\n(Exhaustive Trees)",
        color="blue",
        alpha=0.6,
        ha="center",
        fontsize=10,
        style="italic",
    )
    ax.text(
        6.5,
        ax.get_ylim()[1] * 0.9,
        "gsDT (N=6..10)\n(Group-Sorted splits)",
        color="green",
        alpha=0.6,
        ha="center",
        fontsize=10,
        style="italic",
    )

    ax.set_ylabel("Throughput (Million Sorts / Second)", fontsize=11)
    ax.set_xlabel("Leaf Array Size (N)", fontsize=11)
    ax.set_title("Leaf-Level Sorting Throughput (IntNaked, Random Distribution)", fontsize=12, fontweight="bold")
    ax.set_xticks(x)
    ax.set_xticklabels(sizes)
    ax.grid(True, which="both", ls="-", color="0.9", alpha=0.5, axis="y")
    ax.legend(loc="upper right", frameon=True)

    plt.tight_layout()
    out_path = report_dir / "fig2_leaf_throughput.svg"
    plt.savefig(out_path, format="svg", bbox_inches="tight")
    plt.close()
    print(f"Generated academic figure 2: {out_path.name}", flush=True)


def generate_fig3_complexity(
    conn: sqlite3.Connection, run_id: int, active_headers: dict[int, Any], report_dir: Path
) -> None:
    """
    Generates Fig 3: Sorter space complexity scaling and worst-case comparison bounds.
    """
    rows = db_query(
        conn,
        """
        SELECT n, object_bytes
        FROM code_size
        WHERE run_id = ?
        ORDER BY n
        """,
        (run_id,),
    )
    obj_sizes = {r["n"]: r["object_bytes"] for r in rows}

    sizes = sorted(list(active_headers.keys()))
    if not sizes:
        print("Warning: No active headers parsed for Fig 3.", file=sys.stderr, flush=True)
        return

    hdr_sizes_kb = [active_headers[sz].header_size_kb for sz in sizes]
    obj_sizes_kb = [obj_sizes.get(sz, 0) / 1024.0 for sz in sizes]

    dtsort_worst_cmp = []
    for sz in sizes:
        r = db_query_one(
            conn,
            """
            SELECT max_cmp
            FROM consistency
            WHERE run_id = ? AND impl = 'PURE_GS' AND n = ?
            LIMIT 1
            """,
            (run_id, sz),
        )
        if r:
            dtsort_worst_cmp.append(r["max_cmp"])
        else:
            gs_worst = {2: 1, 3: 3, 4: 5, 5: 7, 6: 11, 7: 13, 8: 17, 9: 20, 10: 23}
            dtsort_worst_cmp.append(gs_worst.get(sz, 0))

    lower_bounds = [OEIS_A036604.get(sz, math.ceil(math.log2(math.factorial(sz)))) for sz in sizes]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

    ax1.plot(sizes, hdr_sizes_kb, label="Header source size (KB)", color="#1f77b4", marker="o", alpha=0.8)
    ax1.plot(sizes, obj_sizes_kb, label="Compiled object size (KB)", color="#ff7f0e", marker="s", alpha=0.8)
    ax1.set_xlabel("Sorter Size (N)", fontsize=11)
    ax1.set_ylabel("Size (KB)", fontsize=11)
    ax1.set_title("Sorter Footprint Scaling", fontsize=12, fontweight="bold")
    ax1.grid(True, which="both", ls="-", color="0.9", alpha=0.5)
    ax1.legend(loc="upper left", frameon=True)
    ax1.set_xticks(sizes)

    ax2.plot(sizes, dtsort_worst_cmp, label="dtsort worst-case comparisons", color="#2ca02c", marker="o", alpha=0.8)
    ax2.plot(
        sizes,
        lower_bounds,
        label="Information Lower Bound (OEIS A036604)",
        color="#7f7f7f",
        linestyle="--",
        marker="x",
        alpha=0.8,
    )
    ax2.set_xlabel("Sorter Size (N)", fontsize=11)
    ax2.set_ylabel("Pairwise Comparisons", fontsize=11)
    ax2.set_title("Worst-Case Comparison Complexity", fontsize=12, fontweight="bold")
    ax2.grid(True, which="both", ls="-", color="0.9", alpha=0.5)
    ax2.legend(loc="upper left", frameon=True)
    ax2.set_xticks(sizes)

    plt.tight_layout()
    out_path = report_dir / "fig3_space_complexity.svg"
    plt.savefig(out_path, format="svg", bbox_inches="tight")
    plt.close()
    print(f"Generated academic figure 3: {out_path.name}", flush=True)
