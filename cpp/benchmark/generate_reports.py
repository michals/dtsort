"""
Unified academic and executive report generator for the dtsort benchmark suite.

Generates:
1. Fig 1: Scale-Up Performance Curve Grid (SVG)
2. Fig 2: Leaf-Level Throughput Grouped Bar Chart (SVG)
3. Fig 3: Space-Complexity & Bounds Comparison (SVG)
4. executive_summary.md (Markdown report)
5. interactive_report.html (Self-contained offline HTML/JS dashboard)
"""

import json
import re
import sqlite3
import sys
from pathlib import Path

from runner.plots import (
    generate_fig1_speedup,
    generate_fig2_throughput,
    generate_fig3_complexity,
)
from runner.reports import (
    SorterCharacteristic,
    generate_executive_summary,
    generate_interactive_dashboard,
)


def parse_active_headers(root_dir: Path, output_dir: Path) -> dict[int, SorterCharacteristic]:
    """
    Analyzes the C++ integration header dt_stable_sort.hh to parse active dtsort targets.

    Resolves worst-case node/leaf sizes from comments and sizes from file statistics.

    Args:
        root_dir: Path to the root project directory.
        output_dir: Path to the compiled output artifacts directory.

    Returns:
        A dictionary mapping sorting array sizes N to SorterCharacteristic NamedTuple instances.
    """
    header_path = root_dir / "include" / "dt_stable_sort.hh"
    if not header_path.exists():
        print(f"Warning: dt_stable_sort.hh not found at {header_path}", file=sys.stderr, flush=True)
        return {}

    content = header_path.read_text(encoding="utf-8")
    includes = re.findall(r"#include\s+\"(dtsort\d+.*?\.hh)\"", content)

    characteristics = {}
    for filename in includes:
        filepath = root_dir / "include" / filename
        if filepath.exists():
            match = re.search(r"dtsort(\d+)", filename)
            if not match:
                continue
            n = int(match.group(1))

            leaves = None
            nodes = None
            try:
                for line in filepath.read_text(encoding="utf-8").splitlines():
                    if "Stats: {" in line:
                        try:
                            stats_json = json.loads(line.split("Stats:")[1].strip())
                            leaves = stats_json.get("leaves")
                            nodes = stats_json.get("nodes")
                        except Exception:
                            pass
                    elif "Leaves:" in line:
                        leaves = int(line.split("Leaves:")[1].strip())
                    elif "Nodes:" in line:
                        nodes = int(line.split("Nodes:")[1].strip())
            except Exception:
                pass

            mode_name = filename.split(f"dtsort{n}_")[-1].replace(".hh", "") if f"dtsort{n}_" in filename else "full"

            characteristics[n] = SorterCharacteristic(
                name=f"dtsort{n}",
                filename=filename,
                mode=mode_name,
                leaves=leaves,
                nodes=nodes,
                header_size_kb=filepath.stat().st_size / 1024.0,
            )

    return characteristics


def generate_all_reports(log_dir_path: str) -> None:
    """
    Drives the reporting pipeline generation from database and parsed headers.

    Args:
        log_dir_path: Path string of the benchmark logs folder.
    """
    log_dir = Path(log_dir_path)
    db_path = log_dir / "results.db"
    if not db_path.exists():
        raise FileNotFoundError(f"results.db not found in {log_dir}")

    report_dir = log_dir / "reports"
    report_dir.mkdir(parents=True, exist_ok=True)

    conn = sqlite3.connect(str(db_path))
    conn.row_factory = sqlite3.Row
    c = conn.cursor()

    try:
        c.execute("SELECT id FROM run_info ORDER BY id DESC LIMIT 1")
        r = c.fetchone()
        if not r:
            raise RuntimeError("No runs found in run_info table.")
        run_id = r[0]

        print(f"\nProcessing database: {db_path}", flush=True)
        print(f"Run ID: {run_id}", flush=True)
        print(f"Destination: {report_dir}", flush=True)

        scripts_dir = Path(__file__).parent.resolve()
        root_dir = scripts_dir.parent.parent
        bench_dir = scripts_dir

        output_dir = root_dir / "cpp" / "output"
        header_root = root_dir / "cpp"

        active_headers = parse_active_headers(header_root, output_dir)

        # 1. Academic SVGs
        generate_fig1_speedup(conn, run_id, report_dir)
        generate_fig2_throughput(conn, run_id, report_dir)
        generate_fig3_complexity(conn, run_id, active_headers, report_dir)

        # 2. Executive markdown
        generate_executive_summary(conn, run_id, active_headers, report_dir)

        # 3. Interactive offline dashboard
        generate_interactive_dashboard(conn, run_id, active_headers, report_dir, bench_dir)

        print("\nReporting pipeline execution complete.", flush=True)

    finally:
        conn.close()


def main() -> None:
    """
    Main entry point for command-line invocation.
    """
    if len(sys.argv) >= 2:
        log_dir = Path(sys.argv[1])
    else:
        # Fall back to latest log directory
        logs_root = Path(__file__).resolve().parent / "logs"
        if not logs_root.exists():
            print("Error: logs root directory not found.", file=sys.stderr, flush=True)
            sys.exit(1)
        subdirs = sorted([d for d in logs_root.iterdir() if d.is_dir()])
        if not subdirs:
            print("Error: No benchmark logs found.", file=sys.stderr, flush=True)
            sys.exit(1)
        log_dir = subdirs[-1]

    generate_all_reports(str(log_dir))


if __name__ == "__main__":
    main()
