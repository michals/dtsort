"""
SQLite database interface and benchmark data ingestion.

Manages SQLite initialization, view registrations, dynamic sizing metadata audits,
and bulk transaction insertions with thorough docstring coverage.
"""

import csv
import json
import sqlite3
from pathlib import Path


def db_query(conn: sqlite3.Connection, sql: str, params: tuple | dict | None = None) -> list[sqlite3.Row]:
    """
    Executes a SELECT query and returns all matching rows as a list of sqlite3.Row.
    """
    c = conn.cursor()
    try:
        if params:
            c.execute(sql, params)
        else:
            c.execute(sql)
        return c.fetchall()
    finally:
        c.close()


def db_query_one(conn: sqlite3.Connection, sql: str, params: tuple | dict | None = None) -> sqlite3.Row | None:
    """
    Executes a SELECT query and returns the first matching row as a sqlite3.Row, or None.
    """
    c = conn.cursor()
    try:
        if params:
            c.execute(sql, params)
        else:
            c.execute(sql)
        return c.fetchone()
    finally:
        c.close()


def setup_db(db_path: Path) -> sqlite3.Connection:
    """
    Initializes a new SQLite database at the specified path and creates all schemas.

    Reads the database schema from the templates/schema.sql template file and
    executes it in the newly created database.

    Args:
        db_path: Absolute Path to the destination SQLite file.

    Returns:
        An open sqlite3.Connection with active views configured.
    """
    conn = sqlite3.connect(str(db_path))
    conn.row_factory = sqlite3.Row
    c = conn.cursor()

    schema_file = Path(__file__).parent.parent.resolve() / "templates" / "schema.sql"
    if not schema_file.exists():
        raise FileNotFoundError(f"Database schema file not found at {schema_file}")

    schema_sql = schema_file.read_text(encoding="utf-8")
    try:
        c.executescript(schema_sql)
    finally:
        c.close()

    conn.commit()
    return conn


def get_dt_sizes(max_n: int, mode: str, bench_dir: Path, root_dir: Path) -> dict[int, dict[str, int]]:
    """
    Dynamically analyzes the size of generated dtsort headers and object files.

    Args:
        max_n: The maximum decision tree size N evaluated.
        mode: The codegen sorting configuration mode.
        bench_dir: Path to the cpp/benchmark directory.
        root_dir: Path to the root project directory.

    Returns:
        A dictionary mapping each N size to its lines, header bytes, and object bytes.
    """
    data = {}
    output_dir = root_dir / "cpp" / "output"
    build_dir = bench_dir / "cpp" / "build"

    for n in range(2, max_n + 1):
        if n <= 5:
            n_mode = "full"
        elif n == 6:
            n_mode = "gs42"
        elif n == 7:
            n_mode = "gs52"
        elif n == 8:
            n_mode = "gs53"
        elif n == 9:
            n_mode = "gs54"
        elif n == 10:
            n_mode = "gs55"
        else:
            n_mode = "full"

        # Search for matched optimal or gs headers
        h_path = output_dir / f"dtsort{n}_{n_mode}.hh"
        if not h_path.exists():
            h_path = output_dir / f"dtsort{n}_full.hh"
        if not h_path.exists():
            h_path = output_dir / f"dtsort{n}.hh"

        if h_path.exists():
            try:
                source_lines = len(h_path.read_text(encoding="utf-8").splitlines())
                source_bytes = h_path.stat().st_size

                # Fetch matching compiled target object file size
                obj_path = build_dir / "CMakeFiles" / f"dt_size_{n}_obj.dir" / f"dt_size_{n}.cc.o"
                object_bytes = obj_path.stat().st_size if obj_path.exists() else 0

                data[n] = {
                    "source_lines": source_lines,
                    "source_bytes": source_bytes,
                    "object_bytes": object_bytes,
                }
            except Exception:
                pass

    return data


def ingest_data(log_dir: Path, conn: sqlite3.Connection, bench_dir: Path, root_dir: Path) -> int:
    """
    Reads all raw data csv/json files under the log directory and inserts them.

    Args:
        log_dir: Path to the specific run logs directory.
        conn: Open SQLite connection.
        bench_dir: Path to the cpp/benchmark directory.
        root_dir: Path to the root project directory.

    Returns:
        The generated run_id reference in run_info table.
    """
    c = conn.cursor()

    # 1. Ingest run_config.json
    config_path = log_dir / "run_config.json"
    if not config_path.exists():
        raise FileNotFoundError(f"Missing run_config.json in {log_dir}. Ingestion aborted.")

    with open(config_path, "r", encoding="utf-8") as f:
        cfg = json.load(f)

    max_n = cfg.get("max_n")
    if max_n is None:
        raise ValueError(f"Missing 'max_n' in {config_path}. Ingestion aborted.")

    mode = cfg.get("mode", cfg.get("dt_mode"))
    if not mode:
        raise ValueError(f"Missing 'mode' or 'dt_mode' in {config_path}. Ingestion aborted.")

    c.execute(
        """INSERT INTO run_info (timestamp, cmdline, git_hash, max_n, mode, count_iters, bench_iters, sizes, dt_block_size, dt_threshold, compiled_mode)
                 VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
        (
            cfg.get("timestamp"),
            cfg.get("cmdline"),
            cfg.get("git_hash"),
            max_n,
            mode,
            cfg.get("count_iters"),
            cfg.get("bench_iters"),
            str(cfg.get("sizes")),
            cfg.get("dt_block_size"),
            cfg.get("dt_threshold"),
            cfg.get("dt_mode"),
        ),
    )
    run_id = c.lastrowid or 1

    # 2. Ingest Consistency CSVs (*.consistency.csv)
    for csv_file in log_dir.glob("*.consistency.csv"):
        if csv_file.name.startswith("pure-"):
            parts = csv_file.name.split("-")
            impl = "PURE_" + parts[1].upper()
            variant = "dtsort"
        else:
            parts = csv_file.name.split("-")
            if len(parts) < 3:
                continue
            impl = parts[1]
            variant = parts[2].split(".")[0]

        try:
            with open(csv_file, "r", encoding="utf-8") as csvf:
                reader = csv.DictReader(csvf)
                for row in reader:
                    if row.get("size") == "size" or not row.get("size"):
                        continue
                    c.execute(
                        """INSERT INTO consistency (run_id, impl, variant, n, avg_cmp, max_cmp, avg_move, max_move)
                                 VALUES (?, ?, ?, ?, ?, ?, ?, ?)""",
                        (
                            run_id,
                            impl,
                            variant,
                            int(row["size"]),
                            float(row["avg_cmp"]),
                            int(row["max_cmp"]),
                            float(row["avg_move"]),
                            int(row["max_move"]),
                        ),
                    )
        except Exception as e:
            print(f"Warning: Failed to ingest consistency file {csv_file.name}: {e}")

    # 3. Ingest Large Counts
    lc_path = log_dir / "large_counts.csv"
    if lc_path.exists():
        try:
            with open(lc_path, "r", encoding="utf-8") as lc_file:
                reader = csv.DictReader(lc_file)
                for row in reader:
                    if row.get("impl") == "impl" or not row.get("impl"):
                        continue
                    impl = row["impl"]
                    if "-" in impl:
                        impl = impl.split("-")[0]
                    c.execute(
                        """INSERT INTO large_counts (run_id, impl, variant, type, size, pattern, avg_cmp, avg_dt_cmp, avg_move, avg_dt_move, max_cmp, max_move)
                                 VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                        (
                            run_id,
                            impl,
                            row["variant"],
                            row["type"],
                            int(row["size"]),
                            row.get("pattern", "random"),
                            float(row["avg_cmp"]),
                            float(row.get("avg_dt_cmp", 0.0)),
                            float(row["avg_move"]),
                            float(row.get("avg_dt_move", 0.0)),
                            int(row.get("max_cmp", 0)),
                            int(row.get("max_move", 0)),
                        ),
                    )
        except Exception as e:
            print(f"Warning: Failed to ingest large counts: {e}")

    # 4. Ingest Google Benchmark JSONs
    for json_file in log_dir.glob("*.benchmark_*.json"):
        try:
            with open(json_file, "r", encoding="utf-8") as jf:
                root = json.load(jf)

            # Check if this benchmark output file has any statistical aggregates
            has_aggregates = any(b.get("run_type") == "aggregate" for b in root.get("benchmarks", []))

            for b in root.get("benchmarks", []):
                if has_aggregates:
                    # Import ONLY the median aggregate
                    if b.get("run_type") != "aggregate" or b.get("aggregate_name") != "median":
                        continue
                else:
                    # Import the raw iteration row
                    if b.get("run_type") != "iteration":
                        continue

                full_name = b["name"]
                if "/" not in full_name:
                    continue

                # Format: BM_Impl/Variant/Type/Pattern[/Size]
                parts = full_name.split("/")
                if len(parts) < 4:
                    continue

                raw_impl = parts[0].replace("BM_", "")
                impl = "dt" if raw_impl == "dt_stable_sort" else raw_impl
                variant = parts[1]
                type_label = parts[2]

                def clean_suffix(s: str) -> str:
                    for suffix in ["_mean", "_median", "_stddev", "_cv"]:
                        if s.endswith(suffix):
                            return s[: -len(suffix)]
                    return s

                pattern = clean_suffix(parts[3])
                size_str = clean_suffix(parts[-1])
                try:
                    size = int(size_str)
                except ValueError:
                    size = int(b.get("size", 0))

                c.execute(
                    """INSERT INTO wall_clock (run_id, impl, variant, type, pattern, size, real_time_ns, cpu_time_ns, iterations)
                             VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                    (
                        run_id,
                        impl,
                        variant,
                        type_label,
                        pattern,
                        size,
                        b.get("real_time", 0.0),
                        b.get("cpu_time", 0.0),
                        b.get("iterations", 0),
                    ),
                )
        except Exception as e:
            print(f"Warning: Failed to ingest wall clock file {json_file.name}: {e}")

    # 5. Ingest Weights
    w_path = log_dir / "weight.csv"
    if w_path.exists():
        try:
            with open(w_path, "r", encoding="utf-8") as w_file:
                reader = csv.DictReader(w_file)
                for row in reader:
                    c.execute(
                        """INSERT INTO type_weights (run_id, type, cmp_ns, move_ns)
                                 VALUES (?, ?, ?, ?)""",
                        (run_id, row["type"], float(row["cmp_ns"]), float(row["move_ns"])),
                    )
        except Exception as e:
            print(f"Warning: Failed to ingest type weights: {e}")

    # 6. Ingest Code Size (reuse max_n and mode from the config already parsed above)
    try:
        sizes_data = get_dt_sizes(max_n, mode, bench_dir, root_dir)
        for n, s in sizes_data.items():
            c.execute(
                """INSERT INTO code_size (run_id, n, source_lines, source_bytes, object_bytes)
                         VALUES (?, ?, ?, ?, ?)""",
                (run_id, n, s["source_lines"], s["source_bytes"], s["object_bytes"]),
            )
    except Exception as e:
        print(f"Warning: Failed to ingest code sizes: {e}")

    conn.commit()
    return run_id


def run_database_pipeline(log_dir_path: str) -> None:
    """
    Executes the database setup and data ingestion pipeline for a logs directory.
    """
    log_dir = Path(log_dir_path)
    db_path = log_dir / "results.db"

    if db_path.exists():
        db_path.unlink()

    conn = setup_db(db_path)
    try:
        bench_dir = Path(__file__).parent.parent.resolve()
        root_dir = bench_dir.parent.parent
        ingest_data(log_dir, conn, bench_dir, root_dir)
    finally:
        conn.close()

    print(f"Created results database: {db_path}", flush=True)
