# dtsort — Benchmark Results & Analysis

This document walks through the performance characteristics of `dt_stable_sort` and explains where it excels, where it's competitive, and why.

All results were measured on **Apple Mac mini M4** (10-core, 32 GB unified memory) against the stock `std::stable_sort` from the same toolchain. The complete dataset is available in the [interactive explorer](https://michals.github.io/dtsort/interactive_report.html) and as raw data in [`cpp/benchmark/results/`](cpp/benchmark/results/).

---

## 1. Heavy Records: Where dtsort Shines Brightest

The most dramatic gains appear when sorting **large, memory-heavy objects**. Our `WideRow` benchmark type is a 4 KB record (64-bit key + 4088-byte payload) — representative of database rows, large structs, or anything where moving data is expensive.

### Peak Performance

| Array Size    | Speedup vs `std::stable_sort` | What's happening                          |
| ------------- | ----------------------------- | ----------------------------------------- |
| N = 290       | **up to 2.4× faster**         | Peak: entire working set fits in L2 cache |
| N = 1,024     | **~1.8× faster**              | Transitioning across L2 cache boundary    |
| N = 10,000    | **~1.3× faster**              | Sustained mid-scale gains                 |
| N = 100,000   | **+10–15%**                   | Sustained macroscopic gains               |
| N = 2,000,000 | **+10%**                      | Sustained at scale                        |

> [→ Explore WideRow speedup curves interactively](https://michals.github.io/dtsort/interactive_report.html#tab=curves&type=WideRow&pattern=random&metric=speedup&minN=25&maxN=2000000&tpType=IntNaked&chart=dt)

### Why It Works

The key innovation for heavy records is **Block-Scoped Index Sorting**:

1. Within cache-resident blocks (default: 160 elements), dtsort sorts a lightweight array of integer indices instead of the heavy payloads.
2. The decision-tree kernels operate on these cheap indices.
3. A single scatter pass moves each payload **exactly once** into its final position — achieving the theoretical minimum of 1.0× N payload moves.

This decoupling means that regardless of how expensive `std::move` is for your type, the sorting logic itself runs at "integer speed." The block size (default `MAX_N × 16 = 160`) is tuned for Apple M-series L2 cache but is configurable via the `DT_BLOCK_SIZE` compile-time macro.

---

## 2. Integer Sorting

For lightweight types like `int` or `int64_t`, dtsort's advantage is concentrated at small array sizes where the decision-tree kernels directly replace the standard library's insertion sort fallback.

### Small-N Performance (N ≤ 10)

At the leaf level, dtsort kernels are dramatically faster:

| N      | Speedup vs stock `std::stable_sort` leaf |
| ------ | ---------------------------------------- |
| N = 2  | ~5× faster                               |
| N = 5  | ~4× faster                               |
| N = 8  | ~2.3× faster                             |
| N = 10 | ~2.6× faster                             |

> [→ Explore small-N throughput interactively](https://michals.github.io/dtsort/interactive_report.html#tab=throughput&type=IntNaked&tpType=IntNaked)

**Why dtsort10 matters most**: Due to the `MAX_N=10` block alignment in `dt_stable_sort`'s recursive split, the vast majority of leaf blocks (all but potentially the last one) are exactly 10 elements wide. This means `dtsort10` — the N=10 kernel using the gs55 Group-Sorted architecture — is the workhorse that drives comparison savings at every scale.

### Large-N Performance

At large scales (N > 10,000), the per-leaf savings are amortized over the O(N log N) merge passes. The gains are modest but consistently positive:

- **Random inputs**: +1–3% sustained at N ≥ 100K
- **Reversed inputs**: +2–3% sustained at N ≥ 100K

> [→ Explore integer speedup curves interactively](https://michals.github.io/dtsort/interactive_report.html#tab=curves&type=IntNaked&pattern=random&metric=speedup&minN=25&maxN=2000000&tpType=IntNaked&chart=dt)

---

## 3. Adaptive Mode & Pre-Sorted Inputs

`dt_stable_sort` ships with **Adaptive Mode enabled by default** (`DT_ADAPTIVE=1`). This is the recommended configuration for a drop-in replacement of `std::stable_sort` because it ensures dtsort is never worse than the standard library on any input distribution.

### How Adaptivity Works

Rather than running a costly pre-scan to detect sorted runs (which would add overhead on random data), adaptivity is embedded directly into the decision tree state machine:

- **Base kernels (N ≤ 5)**: Return a boolean flag indicating if elements were already sorted — at near-zero additional cost.
- **Group-sorted kernels (N ≥ 6)**: Check sorted flags from their pre-sorted sub-groups and perform a single boundary comparison. If everything is already in order, the entire merge phase is bypassed.
- **Recursive engine**: Each merge step checks `*(mid-1) <= *mid` before merging. For sorted or nearly-sorted inputs, this short-circuits the merge entirely.

### When to Disable Adaptivity

If your application processes **exclusively high-entropy (fully random) data** and you prefer a **predictable O(N log N) execution profile** for any input permutation, you can disable adaptivity for a small additional speed boost:

```cpp
#define DT_ADAPTIVE 0
#include "dt_stable_sort.hh"
```

> [→ Compare Adaptive vs High Entropy mode on sorted inputs](https://michals.github.io/dtsort/interactive_report.html#tab=curves&type=IntNaked&pattern=sorted&metric=speedup&minN=25&maxN=2000000&tpType=IntNaked&chart=dt)

### Performance on Pre-Sorted Inputs

With Adaptive Mode enabled, `dt_stable_sort` handles sorted inputs extremely well:

| Input Pattern        | Peak Speedup (Adaptive) | At Scale              |
| -------------------- | ----------------------- | --------------------- |
| Sorted integers      | **up to 4.4× faster**   | Competitive or faster |
| Reversed integers    | **up to 1.4× faster**   | +2–3% sustained       |
| Sorted heavy records | **up to 2.6× faster**   | +10–12% sustained     |

> [→ Explore sorted input performance](https://michals.github.io/dtsort/interactive_report.html#tab=curves&type=IntNaked&pattern=sorted&metric=speedup&minN=25&maxN=2000000&tpType=IntNaked&chart=dt)

---

## 4. Sorter Characteristics

Each generated kernel is mathematically verified. For N=2..5, the kernels achieve the information-theoretic minimum number of comparisons. For N=6..10, the Group-Sorted (gsDT) architecture accepts a tradeoff of +1 worst-case comparison in exchange for orders-of-magnitude smaller code.

| Sorter     | Architecture | Worst Cmp | Avg Cmp | Avg Move | Max Move | Header Size | Object Size |
| ---------- | ------------ | --------- | ------- | -------- | -------- | ----------- | ----------- |
| `dtsort2`  | full         | **1**     | 1.00    | 1.50     | 3        | 1.2 KB      | 0.6 KB      |
| `dtsort3`  | full         | **3**     | 2.67    | 2.83     | 4        | 1.4 KB      | 0.7 KB      |
| `dtsort4`  | full         | **5**     | 4.67    | 4.08     | 6        | 2.4 KB      | 1.1 KB      |
| `dtsort5`  | full         | **7**     | 6.93    | 5.28     | 7        | 6.9 KB      | 3.2 KB      |
| `dtsort6`  | gs42         | 11        | 10.00   | 9.47     | 14       | 2.4 KB      | 1.7 KB      |
| `dtsort7`  | gs52         | 13        | 12.48   | 11.24    | 16       | 2.9 KB      | 3.8 KB      |
| `dtsort8`  | gs53         | 17        | 15.89   | 13.71    | 19       | 5.4 KB      | 5.0 KB      |
| `dtsort9`  | gs54         | 20        | 19.08   | 15.95    | 22       | 11.9 KB     | 7.7 KB      |
| `dtsort10` | gs55         | 23        | 22.08   | 18.06    | 25       | 25.9 KB     | 14.3 KB     |

**Bold** worst-case comparison counts indicate the information-theoretic minimum ([OEIS A036604](https://oeis.org/A036604)) is achieved exactly. The complete N=2..10 suite totals ~56 KB of generated C++ headers — small enough to fit entirely in the L1 instruction cache.

> [→ Explore sorter characteristics interactively](https://michals.github.io/dtsort/interactive_report.html#tab=sorters)

---

## 5. Experimental: Patching GCC & LLVM `std::stable_sort`

As a proof-of-concept, we also patched the leaf-level fallbacks of production `std::stable_sort` implementations to use dtsort kernels instead of their built-in insertion sort:

- **GCC 16** (libstdc++) — replaces the insertion sort fallback for N ≤ 10
- **LLVM 19** (libc++) — replaces the Timsort leaf fallback for N ≤ 10

> **⚠️ These are quick-and-dirty experiments, not production-ready patches.** The patching mechanism downloads and instruments standard library headers into isolated binaries. The purpose is to demonstrate the performance headroom available if standard library maintainers adopted decision-tree leaf sorting.

### Results

The patched standard libraries show consistent improvements, particularly for heavy records:

| Implementation    | WideRow Peak Speedup  | IntNaked Sustained (N ≥ 100K) |
| ----------------- | --------------------- | ----------------------------- |
| GCC 16 (patched)  | **up to 2.3× faster** | +1–3%                         |
| LLVM 19 (patched) | **up to 1.6× faster** | +1–2%                         |

> [→ Explore GCC 16 patched results](https://michals.github.io/dtsort/interactive_report.html#tab=curves&type=WideRow&pattern=random&metric=speedup&minN=25&maxN=2000000&tpType=IntNaked&chart=gcc16)
>
> [→ Explore LLVM 19 patched results](https://michals.github.io/dtsort/interactive_report.html#tab=curves&type=WideRow&pattern=random&metric=speedup&minN=25&maxN=2000000&tpType=IntNaked&chart=llvm19)

---

## 6. Reproducing Everything

All results can be reproduced on your own hardware. See the main [README](README.md) for build prerequisites and benchmark instructions.

Quick start:

```bash
# Quick operation-count sweep (minutes)
make bench-fast

# Full benchmark (several hours on Apple M4)
make bench
```

The benchmark orchestrator generates an interactive HTML report identical in format to the [published one](https://michals.github.io/dtsort/interactive_report.html).

For advanced options:

```bash
cd cpp/benchmark
uv run main.py --help
```
