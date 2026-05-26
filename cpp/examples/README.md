# dtsort Examples

This directory contains standalone, illustrative C++ examples showing how to use `dt_stable_sort` and how it performs compared to the standard library's `std::stable_sort`.

---

## 1. Simple Sort (`simple_sort.cc`)

A basic demonstration of how to integrate `dt_stable_sort` into your codebase. It shows:
* **Sorting fundamental types** (integers) using the default comparator.
* **Sorting custom structs** with custom lambda comparators.
* Preserving element order for equal keys (**stability verification**).

### How to Run:
```bash
cmake -B build && cmake --build build
./build/simple_sort
```

---

## 2. Simple Benchmark (`simple_benchmark.cc`)

A lightweight benchmark to demonstrate the performance benefits of `dt_stable_sort` without setting up the complex, full-scale benchmarking suite. 

It generates **500 random custom objects** and performs **1,000 sort iterations** (using freshly copied unsorted data on each run). It compares `std::stable_sort` with `dt_stable_sort` under the same conditions, verifies correctness and stability, and prints the speedup factor.

### How to Run:
```bash
# Build the benchmark (runs in Adaptive mode by default)
cmake -B build && cmake --build build
./build/simple_benchmark

# Optional: Run the benchmark in High Entropy Mode (optimal for purely random distributions)
cmake -B build -DCMAKE_CXX_FLAGS="-DDT_ADAPTIVE=0" && cmake --build build --clean-first
./build/simple_benchmark
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
