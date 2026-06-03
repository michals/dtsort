# DTSort Benchmark Suite

This directory contains the benchmarking suite for `dtsort`. It supports validation, operational metrics collection, and high-fidelity timing performance measurements of the standalone `dt_stable_sort` implementation as well as multiple standard library implementations (GCC 7, GCC 14, GCC 16, LLVM 15, LLVM 19) patched with `dtsort` leaf sorters.

## Executables

The build process produces separate executables for each standard library implementation and variant (stock vs. dtsort). For example:

1.  **`bench-GCC14-stock`**: Google Benchmark executable wrapping the stock (unpatched) GCC 14 `std::stable_sort()`.
2.  **`bench-GCC14-dtsort`**: Google Benchmark executable wrapping the GCC 14 `std::stable_sort()` patched to use `dtsort` for leaf cases.

Each executable supports multiple modes:
*   **`validate`**: Runs correctness tests comparing the sorted output against `std::stable_sort` on randomized inputs.
*   **`counts`**: Measures and prints the precise number of comparison and assignment operations performed during sorting.
*   **`consistency`**: Verifies that the adaptive mode and high-entropy mode of `dtsort` produce identical sorting states.
*   **`benchmark`**: Runs microbenchmarks using Google Benchmark to measure wall-clock CPU time, cycles, and instructions.

## Prerequisites

- CMake 3.16+
- C++20 compliant compiler (GCC 10+, Clang 12+, AppleClang 13+)
- Internet connection (required to fetch Google Benchmark via CMake)
- Python 3.11+ with the **`uv`** package manager (for automated benchmark orchestration)

## Building

All executables are built using a single CMake configuration.

1.  **Navigate to the benchmark directory:**

    ```bash
    cd cpp/benchmark
    ```

2.  **Create and enter a build directory:**

    ```bash
    mkdir -p build
    cd build
    ```

3.  **Configure the project:**
    (Release mode is strongly recommended for `dt_benchmark` to ensure meaningful timing)

    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

4.  **Build all targets:**
    ```bash
    cmake --build .
    ```
    _Note: The first build will download and configure Google Benchmark, which may take a few moments._

## Running

### Validation

Run the suite to confirm all implementations are working correctly:

```bash
./bench-GCC14-dtsort validate
```

Expected output:

```
Validating GCC14-dtsort...
GCC14-dtsort: ALL PASS
```

### Metrics (Counts)

Generate CSV data for analysis:

```bash
./bench-GCC14-dtsort counts --sizes=100 > counts.csv
```

### Performance Benchmarks

Run the benchmark mode:

```bash
./bench-GCC14-dtsort benchmark
```

Or run a specific subset using filters (regex):

```bash
./bench-GCC14-dtsort benchmark --benchmark_filter="GCC14_IntNaked_stock/8"
```

## Automation

The benchmark process is fully automated via a Python-based orchestrator and reporting pipeline. All stages execute in-process or via structured sub-commands.

### `main.py` (Orchestrator)

This is the primary entry point for executing the entire benchmark lifecycle. It coordinates:
1.  **Regeneration**: Invokes `dtsort-codegen` to produce optimal C++ sorting headers.
2.  **Rebuild & Patching**: Performs in-process standard library patching for GCC 7/14/16 and LLVM 15/19, and rebuilds CMake benchmark binaries.
3.  **Parallel Execution**: Runs non-timing-sensitive stages (`counts`, `consistency`, `validate`) in parallel across multiple CPU cores.
4.  **Sequential Execution**: Runs noise-sensitive wall-clock benchmarks and weight analysis sequentially.
5.  **Thermal Monitoring**: Detects and warns about thermal throttling to ensure timing fidelity.
6.  **In-Process Reporting**: Setup SQLite database, ingest results, and generates timing reports automatically!

Usage:
```bash
uv run main.py [OPTIONS]
```

**Options:**
*   `-n, --max-n MAX_N`: Max dtsort chunk size to benchmark (default: `10`).
*   `-c, --count-iters COUNT_ITERS`: Number of random iterations for counts mode (default: `20`).
*   `-b, --bench-iters BENCH_ITERS`: Statistical repetitions for wall-clock benchmarks (default: `7`).
*   `-s, --skip-build`: Skip code regeneration and CMake build.
*   `--stages STAGES`: Comma-separated stages to execute. Supported: `validate`, `counts`, `consistency`, `weight`, `benchmark_small`, `benchmark_large` (or `all` to run all, default: `all`).
*   `--only-counts`: Shorthand helper to skip time-consuming wall-clock timing runs (equivalent to running only `validate,counts,consistency` stages).
*   `--impls IMPLS`: Comma-separated implementations: `GCC14`, `GCC16`, `GCC7`, `LLVM15`, `LLVM19`, `DT` (default: `DT`).
*   `--types TYPES`: Comma-separated data types: `IntNaked`, `MultiKey`, `WideRow`, `LongPrefix`, `RevDomain`, `CaseInsens` (default: `IntNaked,WideRow`).
*   `--sizes SIZES`: Override large N timing ranges in `count:min:max` spacing format (default: `100:20:2000000`).
*   `--inputs INPUTS`: Comma-separated distributions: `random`, `reversed`, `sorted` (default: `random,reversed,sorted`).
*   `--adaptivity ADAPTIVITY`: Comma-separated variants: `entropy`, `adaptive` (default: `adaptive`).
*   `--dry-run`: Estimate and display total suite runtime without executing actual benchmarks.

Example:
```bash
# Execute a full timing dry-run
uv run main.py --dry-run
```

---

### `generate_reports.py`

Generates publication-quality timing reports and dashboards:
1. **SVG Timing Graphs** (`plots/`): Fig 1 (Scale-Up timing curve), Fig 2 (Throughput grouped bars), Fig 3 (Space-complexity vs lower bounds).
2. **Academic Executive Summary** (`executive_summary.md`): Markdown timings analysis.
3. **Interactive timing Dashboard** (`interactive_report.html`): Offline-capable HTML/JS Chart.js dashboard.

Usage:
```bash
uv run generate_reports.py <log_dir>
```

## Data Types

The benchmark suite uses diverse data types to evaluate different aspects of sorting performance:

1.  **`IntNaked`**: Trivial comparison (`<`) and trivial move/copy (8 bytes). Represents the simplest case where comparison overhead is minimal. Instrumented as `IntElement` for operation counts; uninstrumented as `IntNaked` for wall-clock benchmarks.
2.  **`MultiKey`**: Expensive comparison (last\_name, first\_name, department, salary) and moderate move. Represents records with complex keys.
3.  **`WideRow`**: Trivial comparison but very expensive move (4KB static payload). Tests the impact of cycle decomposition vs. traditional swap strategies.
4.  **`VarString`**: Moderate comparison and move (heap allocation). Represents typical string sorting.
5.  **`LongPrefix`**: Very expensive comparison due to a long shared prefix (64+ chars). Forces deep comparison logic.
6.  **`RevDomain`**: Realistic comparison-heavy data (reversed DNS names like `com.google.www`).
7.  **`CaseInsens`**: Non-trivial comparison logic using case-insensitive rules.
8.  **`SharedPtr`**: Moderate move cost due to atomic refcount updates.

### Input Patterns

- **random**: Uniform random distribution of elements.
- **reversed**: Data sorted in descending order — typically a "worst-case" scenario for many common sorting algorithms, but optimized under adaptive mode.
- **sorted**: Data already sorted in ascending order — allows immediate early-exit in adaptive mode.
