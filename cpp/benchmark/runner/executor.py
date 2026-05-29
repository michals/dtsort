"""
Task execution orchestrator for the benchmark suite.

Manages scheduling of parallel validation/operation counts (high CPU utilization)
and sequential wall-clock benchmarks (low noise, CPU-exclusive). Includes
dry-run estimation and ETA calculation engines.
"""

import concurrent.futures
import datetime
import math
import subprocess
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from typing import NamedTuple, Optional

from runner.config import RunConfig, get_variants, parse_sizes
from runner.consts import DEFAULT_LARGE_SIZES, BenchmarkStage


class BenchmarkCase(NamedTuple):
    impl: str
    variant: str
    type_lbl: str
    pattern: str
    size: int

    @classmethod
    def parse(cls, case_name: str) -> Optional["BenchmarkCase"]:
        """
        Parses a Google Benchmark test case name into a BenchmarkCase named tuple.
        Format is expected to be: impl/variant/type/pattern/size (e.g. dt_stable_sort/stock/IntNaked/random/20)
        """
        parts = case_name.split("/")
        if len(parts) < 5:
            return None
        try:
            size = int(parts[4])
        except ValueError:
            return None
        return cls(impl=parts[0], variant=parts[1], type_lbl=parts[2], pattern=parts[3], size=size)


def _format_duration(seconds: float) -> str:
    h = int(seconds // 3600)
    m = int((seconds % 3600) // 60)
    s = int(seconds % 60)
    parts = []
    if h > 0:
        parts.append(f"{h}h")
    if m > 0 or h > 0:
        parts.append(f"{m}m")
    parts.append(f"{s}s")
    return " ".join(parts)


def estimate_binary_stage_runtime(
    config: RunConfig,
    build_dir: Path,
    bin_name: str,
    stage: BenchmarkStage,
) -> Optional[float]:
    """
    Dynamically estimates the runtime (in seconds) of a specific benchmark stage for a binary.
    """
    exe = build_dir / bin_name
    if not exe.exists() or stage not in (BenchmarkStage.BENCHMARK_SMALL, BenchmarkStage.BENCHMARK_LARGE):
        return None

    try:
        base_args = [str(exe), "--stage=benchmark", "--benchmark_list_tests=true"]
        if config.types:
            base_args.append(f"--types={config.types}")
        if config.inputs:
            base_args.append(f"--inputs={config.inputs}")

        if stage == BenchmarkStage.BENCHMARK_SMALL:
            args = base_args + ["--small-only"]
        else:
            args = base_args + ["--large-only"]
            sizes_expanded = config.sizes or parse_sizes(DEFAULT_LARGE_SIZES)
            sizes_str = ",".join(str(s) for s in sizes_expanded)
            args.append(f"--sizes={sizes_str}")

        res = subprocess.run(args, cwd=build_dir, capture_output=True, text=True, check=True)
        cases = [line.strip() for line in res.stdout.splitlines() if line.strip()]

        type_weights = {
            "IntNaked": 1.0,
            "MultiKey": 4.0,
            "WideRow": 72.0,
            "LongPrefix": 3.0,
            "RevDomain": 2.5,
            "CaseInsens": 3.5,
        }

        rep_count = config.bench_iters
        est_seconds = 0.0
        for case_name in cases:
            case = BenchmarkCase.parse(case_name)
            if not case:
                est_seconds += rep_count * 0.70
                continue
            w = type_weights.get(case.type_lbl, 2.0)
            t_iter = 1.8e-9 * case.size * math.log2(case.size) * w if case.size > 1 else 0.0
            t_timed = max(0.68, t_iter)
            t_alloc = 2.5e-9 * case.size * w
            t_pre_sort = t_iter if case.pattern in ("sorted", "reverse", "reversed") else 0.0
            est_seconds += rep_count * (t_timed + t_alloc + t_pre_sort)

        return est_seconds
    except Exception:
        return None


def _build_common_args(config: RunConfig, exe: Path, stage: BenchmarkStage) -> list[str]:
    """
    Builds the common CLI argument list for a benchmark binary invocation.

    Assembles standard flags shared across all stages: --stage, --types, --inputs.
    Sizes are only added when explicitly overridden by the user.

    Args:
        config: The RunConfig configuration instance.
        exe: Path to the target executable.
        stage: The benchmark stage name.

    Returns:
        A list of CLI argument strings.
    """
    args = [str(exe), f"--stage={stage}"]
    if config.types:
        args.append(f"--types={config.types}")
    if config.inputs:
        args.append(f"--inputs={config.inputs}")
    if config.sizes_explicit:
        sizes_str = ",".join(str(s) for s in config.sizes)
        args.append(f"--sizes={sizes_str}")
    return args


def run_binary_stage(
    config: RunConfig,
    build_dir: Path,
    log_dir: Path,
    binary_name: str,
    stage: BenchmarkStage,
    file_lock: threading.Lock,
) -> str:
    """
    Executes a single benchmark stage for a target compiled binary.

    Handles argument assembly, streams or captures output depending on the stage type,
    and isolates run failures to keep the rest of the benchmark suite running.

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the CMake C++ build directory.
        log_dir: Path to the active logs directory.
        binary_name: Filename of the target binary to run (e.g. "bench-GCC14-dtsort").
        stage: The stage to execute (e.g. BenchmarkStage.COUNTS).
        file_lock: Thread lock for synchronizing access to shared log files (validate.log).

    Returns:
        A status string detailing stage success or failure and execution duration.
    """
    exe = build_dir / binary_name
    if not exe.exists():
        return f"Binary {binary_name} not found"

    start_time = time.perf_counter()
    try:
        if stage == BenchmarkStage.VALIDATE:
            args = _build_common_args(config, exe, BenchmarkStage.VALIDATE)
            res = subprocess.run(args, cwd=build_dir, capture_output=True, text=True, check=True)
            with file_lock:
                log_file = log_dir / "validate.log"
                with log_file.open("a", encoding="utf-8") as f:
                    f.write(f"--- {binary_name} ---\n{res.stdout}\n")
            status = "OK"

        elif stage == BenchmarkStage.COUNTS:
            args = _build_common_args(config, exe, BenchmarkStage.COUNTS)
            args.append(f"--count-iters={config.count_iters}")
            res = subprocess.run(args, cwd=build_dir, capture_output=True, text=True, check=True)
            (log_dir / f"{binary_name}.counts.csv").write_text(res.stdout, encoding="utf-8")
            status = "OK"

        elif stage == BenchmarkStage.CONSISTENCY:
            args = _build_common_args(config, exe, BenchmarkStage.CONSISTENCY)
            res = subprocess.run(args, cwd=build_dir, capture_output=True, text=True, check=True)
            (log_dir / f"{binary_name}.consistency.csv").write_text(res.stdout, encoding="utf-8")
            status = "OK"

        elif stage == BenchmarkStage.WEIGHT:
            args = _build_common_args(config, exe, BenchmarkStage.WEIGHT)
            res = subprocess.run(args, cwd=build_dir, capture_output=True, text=True, check=True)
            (log_dir / f"{binary_name}.weight.csv").write_text(res.stdout, encoding="utf-8")
            status = "OK"

        elif stage in (BenchmarkStage.BENCHMARK_SMALL, BenchmarkStage.BENCHMARK_LARGE):
            args = [str(exe), "--stage=benchmark"]
            if config.types:
                args.append(f"--types={config.types}")
            # Always provide sizes for benchmark stages (expand default if not explicit)
            sizes_expanded = config.sizes or parse_sizes(DEFAULT_LARGE_SIZES)
            sizes_str = ",".join(str(s) for s in sizes_expanded)
            args.append(f"--sizes={sizes_str}")
            args.append(f"--bench-iters={config.bench_iters}")
            if config.inputs:
                args.append(f"--inputs={config.inputs}")

            if stage == BenchmarkStage.BENCHMARK_SMALL:
                args.append("--small-only")
            elif stage == BenchmarkStage.BENCHMARK_LARGE:
                args.append("--large-only")

            output_file = log_dir / f"{binary_name}.{stage}.json"
            args.extend([f"--benchmark_out={output_file}", "--benchmark_out_format=json"])

            # Run with stdout visible to show Google Benchmark progress cleanly
            subprocess.run(args, cwd=build_dir, check=True)
            status = "OK"

        else:
            status = f"Unknown stage {stage}"

        duration = int(time.perf_counter() - start_time)
        return f"{status}: {binary_name} {stage} ({duration}s)"

    except Exception as e:
        duration = int(time.perf_counter() - start_time)
        return f"FAIL: {binary_name} {stage} ({e}) ({duration}s)"


def estimate_runtime(config: RunConfig, build_dir: Path, is_dry_run_cli: bool = True) -> tuple[int, int, int]:
    """
    Simulates the benchmark run configurations, calculating total tests and ETA.

    Utilizes empirically calibrated timings (e.g. ~0.70s per timing iteration)
    to compute expected wall-clock duration without executing the actual benchmarks.

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the CMake C++ build directory.
        is_dry_run_cli: True if invoked by user via `--dry-run` CLI flag.

    Returns:
        A tuple of (total_unique_tests, total_runs, est_total_seconds).
    """
    header_title = (
        "dtsort Benchmark Suite - Dry-Run Estimation"
        if is_dry_run_cli
        else "dtsort Benchmark Suite - Execution Estimation"
    )
    print("==============================================")
    print(f"{header_title}")
    print("==============================================")

    # Collect list of all (impl, variant) pairs to run
    run_tasks = []
    for impl in config.impls:
        for variant in get_variants(impl, config.adaptivity):
            run_tasks.append((impl, variant))

    total_unique_tests = 0
    missing_binaries = []
    est_bench_seconds = 0.0
    rep_count = config.bench_iters

    # Type weights based on relative execution and copy costs
    type_weights = {
        "IntNaked": 1.0,
        "MultiKey": 4.0,
        "WideRow": 72.0,
        "LongPrefix": 3.0,
        "RevDomain": 2.5,
        "CaseInsens": 3.5,
    }

    for impl, variant in run_tasks:
        bin_name = f"bench-{impl}-{variant}"
        exe = build_dir / bin_name
        if not exe.exists():
            missing_binaries.append(bin_name)
            continue

        # Common arguments to list tests
        base_args = [str(exe), "--stage=benchmark", "--benchmark_list_tests=true"]
        if config.types:
            base_args.append(f"--types={config.types}")
        if config.inputs:
            base_args.append(f"--inputs={config.inputs}")

        # 1. Estimate small N
        small_cases = []
        if BenchmarkStage.BENCHMARK_SMALL in config.stages:
            small_args = base_args + ["--small-only"]
            try:
                res = subprocess.run(small_args, cwd=build_dir, capture_output=True, text=True, check=True)
                small_cases = [line.strip() for line in res.stdout.splitlines() if line.strip()]
            except Exception as e:
                print(f"  Warning: Failed to query small cases for {bin_name}: {e}")

        # 2. Estimate large N
        large_cases = []
        if BenchmarkStage.BENCHMARK_LARGE in config.stages:
            large_args = base_args + ["--large-only"]
            sizes_expanded = config.sizes or parse_sizes(DEFAULT_LARGE_SIZES)
            sizes_str = ",".join(str(s) for s in sizes_expanded)
            large_args.append(f"--sizes={sizes_str}")

            try:
                res = subprocess.run(large_args, cwd=build_dir, capture_output=True, text=True, check=True)
                large_cases = [line.strip() for line in res.stdout.splitlines() if line.strip()]
            except Exception as e:
                print(f"  Warning: Failed to query large cases for {bin_name}: {e}")

        num_small = len(small_cases)
        num_large = len(large_cases)
        total_unique_tests += num_small + num_large
        if num_small or num_large:
            print(f"  Binary {bin_name:30} -> Small: {num_small:4} cases, Large: {num_large:4} cases")

        # Estimate duration for each case in the binary
        for case_name in small_cases + large_cases:
            case = BenchmarkCase.parse(case_name)
            if not case:
                est_bench_seconds += rep_count * 0.70
                continue

            w = type_weights.get(case.type_lbl, 2.0)

            # Timed loop iteration time: t_sort = 1.8e-9 * N * log2(N) * weight
            t_iter = 1.8e-9 * case.size * math.log2(case.size) * w if case.size > 1 else 0.0
            t_timed = max(0.68, t_iter)

            # Setup/teardown allocation & copy cost
            t_alloc = 2.5e-9 * case.size * w

            # Pre-sorting cost in setup
            t_pre_sort = t_iter if case.pattern in ("sorted", "reverse", "reversed") else 0.0

            est_bench_seconds += rep_count * (t_timed + t_alloc + t_pre_sort)

    total_runs = total_unique_tests * rep_count

    # Other stages (validate, counts, consistency, weight)
    # Validate = ~1.0s/variant, Counts = ~2.5s/variant, Consistency = ~0.5s/variant, Weight = ~1.5s total
    active_tasks = len(run_tasks) - len(missing_binaries)
    stage_weights = {
        BenchmarkStage.VALIDATE: 1.0,
        BenchmarkStage.COUNTS: 2.5,
        BenchmarkStage.CONSISTENCY: 0.5,
    }
    other_time_per_variant = sum(stage_weights[s] for s in stage_weights if s in config.stages)
    est_other_seconds = active_tasks * other_time_per_variant
    if BenchmarkStage.WEIGHT in config.stages:
        est_other_seconds += 1.5
    # Ensure a small positive estimate for active/dry-run configurations
    if est_other_seconds == 0.0 and len(run_tasks) > 0 and len(config.stages) > 0:
        est_other_seconds = 1.0

    build_time_str = "1m 0s" if not config.skip_build else "0s"
    est_total_seconds = est_bench_seconds + est_other_seconds

    # If we are in dry-run mode, include the build time in the total expected runtime estimate.
    # If we are in regular execution mode, the build has already completed, so we estimate from now.
    if is_dry_run_cli and not config.skip_build:
        est_total_seconds += 60.0

    now = datetime.datetime.now()
    eta = now + datetime.timedelta(seconds=est_total_seconds)
    eta_str = eta.strftime("%H:%M:%S (%Y-%m-%d)" if eta.day != now.day else "%H:%M:%S")

    print("==============================================")
    print("Summary of Expected Runtime:")
    print("==============================================")
    print(f"  Target Binaries:              {len(run_tasks)} ({len(missing_binaries)} missing)")
    if missing_binaries:
        print(f"  Missing Binaries:             {', '.join(missing_binaries)}")
        print("                                (Run without --skip-build to compile them first)")
    print(f"  Unique Test Configurations:   {total_unique_tests}")
    print(f"  Repetitions per test:         {rep_count}")
    print(f"  Total Wall-Clock Test Runs:   {total_runs}")
    print("----------------------------------------------")
    if is_dry_run_cli:
        print(f"  Estimated Build Time:         {build_time_str}")
    print(f"  Estimated Verify/Counts Time: {_format_duration(est_other_seconds)}")
    print(f"  Estimated Benchmark Time:     {_format_duration(est_bench_seconds)}")
    print("  --------------------------------------------")
    print(f"  TOTAL EXPECTED RUN TIME:      {_format_duration(est_total_seconds)}")
    print(f"  ESTIMATED COMPLETION (ETA):   {eta_str}")
    print("==============================================")

    return total_unique_tests, total_runs, int(est_total_seconds)


def execute_stages(config: RunConfig, build_dir: Path, log_dir: Path) -> int:
    """
    Executes the entire orchestration workflow stages across all target implementations.

    Invokes parallel queues for high-CPU verification tasks and strict sequential execution
    for performance testing to eliminate run noise.

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the CMake C++ build directory.
        log_dir: Path to the active logs directory.

    Returns:
        The total elapsed suite execution time in seconds.
    """
    suite_start = time.perf_counter()

    # Collect list of all (impl, variant) pairs to run
    run_tasks = []
    for impl in config.impls:
        for variant in get_variants(impl, config.adaptivity):
            run_tasks.append((impl, variant))

    total_tasks = len(run_tasks)
    completed_small = 0
    completed_large = 0

    num_impls = len(config.impls)
    weight_run_done = False
    file_lock = threading.Lock()

    for i, impl in enumerate(config.impls):
        print(f"--- Implementation: {impl} ({i + 1}/{num_impls}) ---")

        for variant in get_variants(impl, config.adaptivity):
            bin_name = f"bench-{impl}-{variant}"
            print(f"  Variant: {variant}")

            # 1. Run parallelizable stages (counts, consistency, validate)
            with ThreadPoolExecutor(max_workers=3) as thread_executor:
                futures = []
                active_parallel_stages = [
                    s for s in [BenchmarkStage.COUNTS, BenchmarkStage.CONSISTENCY] if s in config.stages
                ]
                if BenchmarkStage.VALIDATE in config.stages:
                    if variant.startswith("dtsort") or (impl == config.impls[0] and variant == "stock"):
                        active_parallel_stages.append(BenchmarkStage.VALIDATE)

                for stage in active_parallel_stages:
                    futures.append(
                        thread_executor.submit(
                            run_binary_stage,
                            config,
                            build_dir,
                            log_dir,
                            bin_name,
                            stage,
                            file_lock,
                        )
                    )

                for future in concurrent.futures.as_completed(futures):
                    print(f"    {future.result()}")

            # 2. Run sequential stages (weight and wall-clock benchmarks)
            if BenchmarkStage.WEIGHT in config.stages and not weight_run_done:
                print(
                    f"    [Sequential] {run_binary_stage(config, build_dir, log_dir, bin_name, BenchmarkStage.WEIGHT, file_lock)}"
                )
                weight_run_done = True

            # Small N timing
            if BenchmarkStage.BENCHMARK_SMALL in config.stages:
                small_n_range = ",".join(str(n) for n in range(2, config.max_n + 1))
                small_label = f"stable_sort for N = {small_n_range}"
                completed_small += 1
                est_sec = estimate_binary_stage_runtime(config, build_dir, bin_name, BenchmarkStage.BENCHMARK_SMALL)
                est_str = f" (estimated runtime: {_format_duration(est_sec)})" if est_sec is not None else ""
                print(
                    f"    [Sequential] Executing SmallN bench {completed_small} of {total_tasks}{est_str}...",
                    flush=True,
                )
                res_small = run_binary_stage(
                    config, build_dir, log_dir, bin_name, BenchmarkStage.BENCHMARK_SMALL, file_lock
                )
                print(f"    [Sequential] {res_small.replace('benchmark_small', small_label)}")

            # Large N timing
            if BenchmarkStage.BENCHMARK_LARGE in config.stages:
                sizes_expanded = config.sizes or parse_sizes(DEFAULT_LARGE_SIZES)
                sizes_str = ",".join(str(s) for s in sizes_expanded)
                large_label = f"stable_sort for N = {sizes_str}"
                completed_large += 1
                est_sec = estimate_binary_stage_runtime(config, build_dir, bin_name, BenchmarkStage.BENCHMARK_LARGE)
                est_str = f" (estimated runtime: {_format_duration(est_sec)})" if est_sec is not None else ""
                print(
                    f"    [Sequential] Executing LargeN bench {completed_large} of {total_tasks}{est_str}...",
                    flush=True,
                )
                res_large = run_binary_stage(
                    config, build_dir, log_dir, bin_name, BenchmarkStage.BENCHMARK_LARGE, file_lock
                )
                print(f"    [Sequential] {res_large.replace('benchmark_large', large_label)}")

    return int(time.perf_counter() - suite_start)


def aggregate_results(log_dir: Path) -> None:
    """
    Consolidates isolated CSV files into master aggregate files for reporting.

    Aggregates per-binary counts and weights files to ensure clean downstream SQLite ingestion.

    Args:
        log_dir: Path to the active logs directory.
    """
    print("Aggregating results...", flush=True)

    # 1. Aggregating large counts
    master_counts = log_dir / "large_counts.csv"
    header_written = False
    with master_counts.open("w", encoding="utf-8") as master:
        for f in sorted(log_dir.glob("*.counts.csv")):
            lines = f.read_text(encoding="utf-8").splitlines()
            if not lines:
                continue
            if not header_written:
                master.write(lines[0] + "\n")
                header_written = True
            for line in lines[1:]:
                if line.strip():
                    master.write(line + "\n")

    # 2. Aggregating type weights
    master_weight = log_dir / "weight.csv"
    header_written = False
    with master_weight.open("w", encoding="utf-8") as master:
        for f in sorted(log_dir.glob("*.weight.csv")):
            lines = f.read_text(encoding="utf-8").splitlines()
            if not lines:
                continue
            if not header_written:
                master.write(lines[0] + "\n")
                header_written = True
            for line in lines[1:]:
                if line.strip():
                    master.write(line + "\n")
            break  # We only take the first compiled implementation's weights as reference
