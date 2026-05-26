"""
CLI utility for generating a markdown benchmark summary from SQLite database results.
"""

import sys
from pathlib import Path

from runner.analysis import run_database_analysis


def main() -> None:
    """
    Main entry point for command-line invocation.
    """
    if len(sys.argv) < 2:
        print("Usage: python3 summary.py <log_dir>", file=sys.stderr)
        sys.exit(1)

    log_dir = Path(sys.argv[1])
    db_path = log_dir / "results.db"
    output_path = log_dir / "summary.md"

    if not db_path.exists():
        print(f"Error: {db_path} not found.", file=sys.stderr)
        sys.exit(1)

    if run_database_analysis(db_path, output_path):
        print(f"Generated benchmark summary: {output_path}", flush=True)
    else:
        if output_path.exists():
            output_path.unlink()
        sys.exit(1)


if __name__ == "__main__":
    main()
