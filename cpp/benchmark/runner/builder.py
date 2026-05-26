"""
C++ source generation and target compilation builders.

Automates C++ header re-generation, standard library patching (patch_impls.sh),
CMake reconfigurations, and parallel toolchain builds under the unified
`execute_command` interface.
"""

import subprocess
from pathlib import Path

from runner.config import RunConfig, get_variants
from runner.consts import BenchmarkImpl
from runner.utils import execute_command


def run_root_generation(config: RunConfig, root_dir: Path, log_file: Path) -> None:
    """
    Triggers C++ dtsort source code generation targets in the root directory.

    Re-runs the 'generate' target to ensure fresh optimal kernels.
    Also executes the pure-gs metrics verification to produce raw bounds datasets.

    Note: 'clean-generated' is intentionally skipped here to preserve any
    pre-shipped headers in cpp/output/ if the build fails partway through.
    Users who need a full clean slate can run `make clean` manually.

    Args:
        config: The RunConfig configuration instance.
        root_dir: Path to the root project directory.
        log_file: Path to the build log file where compiler output is written.
    """
    print("Building dtsort tools and generating headers (mode=gs)...", flush=True)

    # Ensure output directory exists so verify/codegen can write to it
    (root_dir / "output").mkdir(parents=True, exist_ok=True)

    # 1. Configure root project
    execute_command(["cmake", "-B", "build"], cwd=root_dir, log_file=log_file)

    # 2. Generate optimal/gs decision trees and headers
    execute_command(
        ["cmake", "--build", "build", "--target", "generate"],
        cwd=root_dir,
        log_file=log_file,
    )

    # 3. Generate pure metrics CSV using dtsort-verify
    verify_exe = root_dir / "build" / "dtsort-verify-am"
    execute_command(
        [
            str(verify_exe),
            "--csv",
            "output/pure-gs.csv",
            "--mode",
            "gs",
            "-n",
            str(config.max_n),
        ],
        cwd=root_dir,
        log_file=log_file,
    )


def run_stdlib_patching(config: RunConfig, bench_dir: Path, log_file: Path) -> None:
    """
    Executes standard library patching routines at the benchmark workspace root.

    Directly downloads original STL headers and injects low-level leaf hooks
    under the target_std namespace, entirely driven by Python logic rather than
    external bash scripts.

    Skipped automatically when only the standalone dt_stable_sort implementation
    is requested (--impls DT), since that target requires no patched stdlib headers
    and the patch scripts rely on Unix shell — they are not available on Windows.

    Args:
        config: The RunConfig configuration instance.
        bench_dir: Path to the cpp/benchmark directory.
        log_file: Path to the build log file.
    """
    dt_only = all(impl == BenchmarkImpl.DT_STABLE_SORT for impl in config.impls)
    if dt_only:
        print("Skipping stdlib patching (DT-only run — no patched stdlib targets needed).", flush=True)
        return

    print("Patching standard library implementations...", flush=True)
    impls_dir = bench_dir / "cpp" / "stdlib_impls"

    for impl in ["gcc7", "gcc14", "gcc16", "llvm15", "llvm19"]:
        impl_dir = impls_dir / impl
        if impl_dir.is_dir():
            print(f"--- Implementation: {impl} ---", flush=True)
            patch_dtsort = impl_dir / "patch_dtsort.sh"
            if patch_dtsort.is_file():
                print("  Running patch_dtsort.sh...", flush=True)
                execute_command(["/bin/bash", "./patch_dtsort.sh"], cwd=impl_dir, log_file=log_file)

            patch_stock = impl_dir / "patch_stock.sh"
            if patch_stock.is_file():
                print("  Running patch_stock.sh...", flush=True)
                execute_command(["/bin/bash", "./patch_stock.sh"], cwd=impl_dir, log_file=log_file)
        else:
            print(f"Warning: Implementation directory {impl_dir} not found, skipping.", flush=True)


def configure_benchmark_suite(config: RunConfig, build_dir: Path, log_file: Path) -> None:
    """
    Runs CMake configuration for the Google Benchmark targets.

    Passes the compile-time constants `DTSORT_MAX_N` and `DT_MODE_GS=ON`.
    When only the standalone DT implementation is requested, also passes
    `-DDTSORT_STDLIB_IMPLS=OFF` so CMake skips declaring the heavy GCC/LLVM
    stdlib targets (which require patched headers from Unix shell scripts).

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the CMake benchmark build directory.
        log_file: Path to the build log file.
    """
    print(f"Configuring benchmark suite (K={config.max_n})...", flush=True)
    build_dir.mkdir(parents=True, exist_ok=True)

    dt_only = all(impl == BenchmarkImpl.DT_STABLE_SORT for impl in config.impls)
    cmake_args = [
        "cmake",
        f"-DDTSORT_MAX_N={config.max_n}",
        "-DDT_MODE_GS=ON",
    ]
    if dt_only:
        cmake_args.append("-DDTSORT_STDLIB_IMPLS=OFF")
    cmake_args.append("..")
    execute_command(cmake_args, cwd=build_dir, log_file=log_file)


def compile_benchmark_targets(config: RunConfig, build_dir: Path, log_file: Path) -> None:
    """
    Executes make to compile stock, patched, and metadata sizing C++ targets.

    Utilizes Popen to stream standard outputs into the log file in real-time,
    while displaying pretty target completion indicators on the console.

    Args:
        config: The RunConfig configuration instance.
        build_dir: Path to the benchmark build directory.
        log_file: Path to the build log file.

    Raises:
        subprocess.CalledProcessError: If make exits with a non-zero status.
    """
    print("Compiling benchmark binaries (parallel)...", flush=True)

    targets = []
    for impl in config.impls:
        for variant in get_variants(impl, config.adaptivity):
            targets.append(f"bench-{impl}-{variant}")

    # Compile sizing metadata targets
    for n in range(2, config.max_n + 1):
        targets.append(f"dt_size_{n}_obj")

    make_cmd = ["make", "-j4"] + targets

    with open(log_file, "a", encoding="utf-8") as out_file:
        out_file.write(f"\n--- [RUN COMPILATION] {' '.join(make_cmd)} ---\n")
        out_file.flush()

        process = subprocess.Popen(
            make_cmd,
            cwd=build_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )

        # Monitor and stream the process output in real-time
        for line in process.stdout:  # type: ignore
            out_file.write(line)
            if "Built target bench-" in line:
                target = line.split("Built target ")[1].strip()
                print(f"  [DONE] {target}", flush=True)

        process.wait()
        if process.returncode != 0:
            raise subprocess.CalledProcessError(process.returncode, "make")
