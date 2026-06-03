# dtsort Examples

This directory contains standalone, illustrative C++ examples showing how to use `dt_stable_sort` and how it performs compared to the standard library's `std::stable_sort`.

---

## 1. Simple Sort (`simple_sort.cc`)

A basic demonstration of how to integrate `dt_stable_sort` into your codebase. It shows:

- **Sorting fundamental types** (integers) using the default comparator.
- **Sorting custom structs** with custom lambda comparators.
- Preserving element order for equal keys (**stability verification**).

### How to Run:

```bash
cmake -B build && cmake --build build
./build/simple_sort
```

---

## 2. Simple Benchmark (`simple_benchmark.cc`)

A lightweight benchmark to demonstrate the performance benefits of `dt_stable_sort` without setting up the complex, full-scale benchmarking suite.

It generates **1234 random custom objects** and performs **1,000 sort iterations** (using freshly copied unsorted data on each run). It compares `std::stable_sort` with `dt_stable_sort` under the same conditions, verifies correctness and stability, and prints the speedup factor.

We compile two separate benchmark binaries for the different adaptivity configurations:

- **`simple_benchmark_adaptive`** (`DT_ADAPTIVE=1`): Adaptive mode, optimal for sorted, reversed, or partially sorted data.
- **`simple_benchmark_entropy`** (`DT_ADAPTIVE=0`): High entropy mode, optimal for purely random distributions.

### How to Run:

```bash
# Build the benchmarks (both Adaptive and Entropy versions are built automatically)
cmake -B build && cmake --build build

# Run with defaults (1234 elements, 1000 runs)
./build/simple_benchmark_adaptive

# Run the High Entropy Mode benchmark
./build/simple_benchmark_entropy

# Optional: Run with custom element count (e.g. 100 elements, default 1000 runs)
./build/simple_benchmark_adaptive 100

# Optional: Run with custom element count and runs count (e.g. 100 elements, 8000 runs)
./build/simple_benchmark_adaptive 100 8000
```

### Example Benchmark Output:

```text
==================================================
     dtsort vs std::stable_sort Benchmark
     Adaptivity Mode (DT_ADAPTIVE): 1
==================================================

Generating 500 random Student objects...
Preparing 1000 copies for std::stable_sort...
Preparing 1000 copies for dt_stable_sort...
Running benchmark...

  [1/2] Benchmarking std::stable_sort()... Done.
  [2/2] Benchmarking dt_stable_sort()...  Done.

Verifying correctness and stability... SUCCESS! (All runs matched std::stable_sort exactly)

----------------- RESULTS -----------------
std::stable_sort:
  Total time:    243.877 ms (for 1000 runs)
  Average/sort:  243.877 us

dt_stable_sort:
  Total time:    221.844 ms (for 1000 runs)
  Average/sort:  221.844 us

Speedup factor: 1.099x
-> dt_stable_sort is 9.932% faster than std::stable_sort!
-------------------------------------------
```
