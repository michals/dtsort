"""
Main entry point and orchestrator for the dtsort benchmark suite.

Coordinates code generation, compiler patching, target binary compilation,
verification, timing execution, and database/report generation in a unified
native Python process. Spawning monolithic Python-to-Python subprocesses is
completely bypassed in favor of in-process functional imports.
"""

import contextlib
import datetime
import io
import json
import subprocess
import sys
from pathlib import Path
from typing import Any

from generate_reports import generate_all_reports
from runner.analysis import run_database_analysis
from runner.builder import (
    compile_benchmark_targets,
    configure_benchmark_suite,
    run_root_generation,
    run_stdlib_patching,
)
from runner.config import RunConfig, get_variants, parse_cli_args
from runner.db import run_database_pipeline
from runner.environment import check_auto_skip_build
from runner.executor import (
    aggregate_results,
    estimate_runtime,
    execute_stages,
)


def get_git_hash(root_dir: Path) -> str:
    """
    Resolves the short Git commit hash of the current active workspace.

    Args:
        root_dir: Path to the root project Git directory.

    Returns:
        The short git commit hash, or 'unknown'.
    """
    try:
        return subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], text=True, cwd=root_dir).strip()
    except Exception:
        return "unknown"


def get_binary_config(config: RunConfig, build_dir: Path) -> dict[str, Any]:
    """
    Interrogates the compiled benchmark binaries to retrieve compile-time configuration limits.

    Runs the binary with `--stage=config` to audit max sizes, block thresholds, etc.

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the CMake C++ build directory.

    Returns:
        A dictionary of the key-value config attributes parsed from stdout.
    """
    for impl in config.impls:
        for variant in get_variants(impl, config.adaptivity):
            if variant == "stock":
                continue
            exe = build_dir / f"bench-{impl}-{variant}"
            if exe.exists():
                break
        else:
            exe = build_dir / f"bench-{impl}-dtsort"
            if exe.exists():
                break
            continue
        break
    else:
        return {}

    bin_config = {}
    try:
        res = subprocess.run([str(exe), "--stage=config"], capture_output=True, text=True, check=True)
        for line in res.stdout.splitlines():
            if "=" in line:
                key, val = line.split("=", 1)
                if val.isdigit():
                    bin_config[key.lower()] = int(val)
                else:
                    bin_config[key.lower()] = val
    except Exception as e:
        print(f"Warning: Could not get config from binary: {e}", file=sys.stderr, flush=True)
    return bin_config


def run_benchmark_orchestration(config: RunConfig) -> None:
    """
    Orchestrates the entire benchmark lifecycle in-process.

    Maintains sequential stages: setup, compile (if not skipped), estimate,
    counts/verification, performance benchmarks, CSV aggregation, and report generation.

    Args:
        config: The RunConfig configuration instance.
    """
    # 1. Setup paths
    bench_dir = Path(__file__).parent.resolve()
    root_dir = bench_dir.parent.parent
    build_dir = bench_dir / "cpp" / "build"
    log_dir = bench_dir / "logs" / datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    git_hash = get_git_hash(root_dir)

    codegen_dir = root_dir / "codegen"
    output_dir = root_dir / "codegen" / "output"

    log_dir.mkdir(parents=True, exist_ok=True)

    # 2. Check build bypass auto-skip heuristics
    if not config.skip_build and check_auto_skip_build(config, build_dir, root_dir):
        print(
            "Auto-Skip Build: Worktree is clean and binaries are up-to-date. Skipping compilation phase.",
            flush=True,
        )
        config.skip_build = True

    # 3. Compile C++ targets
    if not config.skip_build:
        build_log_path = log_dir / "build.log"
        try:
            run_root_generation(config, codegen_dir, build_log_path)
            run_stdlib_patching(config, bench_dir, build_log_path)
            configure_benchmark_suite(config, build_dir, build_log_path)
            compile_benchmark_targets(config, build_dir, build_log_path)
        except Exception as e:
            print(f"\nERROR: Build failed: {e}", file=sys.stderr)
            print(f"Check build log for details: {build_log_path}", file=sys.stderr)
            sys.exit(1)
    else:
        # If skipping build, audit compiled DTSORT_MAX_N to override config if needed
        cmake_cache = build_dir / "CMakeCache.txt"
        if cmake_cache.exists():
            for line in cmake_cache.read_text(encoding="utf-8").splitlines():
                if line.startswith("DTSORT_MAX_N:STRING="):
                    try:
                        compiled_max_n = int(line.strip().split("=")[1])
                        if compiled_max_n != config.max_n:
                            print(
                                f"\nWARNING: --skip-build passed but CLI max_n={config.max_n} != compiled DTSORT_MAX_N={compiled_max_n}."
                            )
                            print(f"Using compiled value: {compiled_max_n}\n")
                            config.max_n = compiled_max_n
                    except (ValueError, IndexError):
                        pass
                    break

    # 4. Dry-Run Check
    if config.dry_run:
        estimate_runtime(config, build_dir, is_dry_run_cli=True)
        return

    # Print execution estimate at the start of active runs
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        estimate_runtime(config, build_dir, is_dry_run_cli=False)
    estimate_output = f.getvalue()
    print(estimate_output, end="")

    # Save the estimate to log_dir / "estimate.txt"
    estimate_file = log_dir / "estimate.txt"
    try:
        estimate_file.write_text(estimate_output, encoding="utf-8")
    except Exception as e:
        print(f"Warning: Could not write estimate.txt: {e}", file=sys.stderr)

    print("==============================================")
    print("dtsort Benchmark Suite Orchestrator")
    print(f"Log Directory: {log_dir}")
    print("==============================================")

    # 5. Run Execution Stages
    suite_seconds = execute_stages(config, build_dir, log_dir)

    # Append the actual duration to the estimate file
    h = suite_seconds // 3600
    m = (suite_seconds % 3600) // 60
    s = suite_seconds % 60
    parts = []
    if h > 0:
        parts.append(f"{h}h")
    if m > 0 or h > 0:
        parts.append(f"{m}m")
    parts.append(f"{s}s")
    duration_str = " ".join(parts)

    actual_duration_msg = (
        f"\n==============================================\n"
        f"Actual Run Duration: {duration_str} ({suite_seconds} seconds)\n"
        f"==============================================\n"
    )
    try:
        with estimate_file.open("a", encoding="utf-8") as ef:
            ef.write(actual_duration_msg)
    except Exception as e:
        print(f"Warning: Could not write actual duration to estimate.txt: {e}", file=sys.stderr)

    # 6. Inject PURE theoretical bounds CSV
    print("--- Implementation: PURE (Theoretical Exhaustive) ---")
    # output_dir dynamically resolved in setup paths
    print("  Variant: gs")
    src_csv = output_dir / "pure-gs.csv"
    dst_csv = log_dir / "pure-gs-dtsort.consistency.csv"
    if src_csv.exists():
        dst_csv.write_text(src_csv.read_text(), encoding="utf-8")
        print(f"    OK: Theoretical metrics copied to {dst_csv.name}", flush=True)
    else:
        print(
            "    SKIP: Theoretical metrics for gs not found in output/. Run without --skip-build to generate.",
            flush=True,
        )

    # 7. Aggregate CSV output files
    aggregate_results(log_dir)

    # 8. Save global run_config.json metadata
    config_data = {
        "timestamp": datetime.datetime.now().isoformat(),
        "cmdline": " ".join(sys.argv),
        "git_hash": git_hash,
        "max_n": config.max_n,
        "count_iters": config.count_iters,
        "bench_iters": config.bench_iters,
        "mode": config.mode,
        "impls": config.impls,
        "types": config.types,
        "duration_sec": suite_seconds,
        "only_counts": config.only_counts,
        "stages": [s.value for s in config.stages],
        "pure_dtsort": True,
        "sizes": ",".join(str(s) for s in config.sizes),
        "inputs": config.inputs,
        "adaptivity": config.adaptivity,
        "adaptivity_levels": [x.strip() for x in config.adaptivity.split(",")],
    }

    # Interrogate binaries for internal parameters
    bin_cfg = get_binary_config(config, build_dir)
    for k, v in bin_cfg.items():
        config_data[k] = v

    (log_dir / "run_config.json").write_text(json.dumps(config_data, indent=2), encoding="utf-8")

    # 9. Native Report Generation Invocations (Eliminating Subprocesses)
    try:
        print("Running in-process reporting pipeline...", flush=True)

        # Setup SQLite tables and views, and ingest CSV/JSON records
        run_database_pipeline(str(log_dir))

        # Generate markdown report summary.md
        run_database_analysis(log_dir / "results.db", log_dir / "summary.md")

        # Generate academic SVGs, executive summary, and HTML dashboard
        generate_all_reports(str(log_dir))
    except Exception as e:
        print(f"Warning: In-process reporting pipeline failed: {e}", file=sys.stderr, flush=True)

    print("==============================================")
    print("All steps completed successfully.")
    print(f"Summary Report: {log_dir / 'summary.md'}")
    print(f"Database: {log_dir / 'results.db'}")
    print("==============================================")


def main() -> None:
    """
    Main entry point for CLI benchmark orchestrator execution.
    """
    config = parse_cli_args()
    run_benchmark_orchestration(config)


if __name__ == "__main__":
    main()
