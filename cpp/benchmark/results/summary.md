# Benchmark Summary: Stock vs. dtsort (DB Driven)

*Note: All benchmarks invoke `target_std::stable_sort()` as the base reference algorithm.*

## 1. Summary

- **Max N (K)**: 10
- **Sort Mode**: gs (Compiled: GS)
- **Block Size**: 160
- **Git Hash**: d462653
- **Command**: `main.py -n 10 --impls DT,GCC16,LLVM19 --inputs random,reversed,sorted --adaptivity adaptive,entropy --sizes 100:20:2000000`
- **Timestamp**: 2026-05-28T06:46:20.606155
- **Iterations (Counts)**: 20

## 2. Pure dtsort

These metrics are calculated by exhaustively iterating through ALL permutations.
This represents the absolute theoretical limit of the dtsort algorithm for small N.

*Source: `consistency` table*

### Mode: GS
| N | Avg Cmp | Max Cmp | Avg Move | Max Move |
|---|---------|---------|----------|----------|
| 2 | 1.000 | 1 | 1.500 | 3 |
| 3 | 2.667 | 3 | 2.833 | 4 |
| 4 | 4.667 | 5 | 4.083 | 6 |
| 5 | 6.933 | 7 | 5.283 | 7 |
| 6 | 10.000 | 11 | 9.467 | 14 |
| 7 | 12.476 | 13 | 11.238 | 16 |
| 8 | 15.893 | 17 | 13.714 | 19 |
| 9 | 19.079 | 20 | 15.952 | 22 |
| 10 | 22.079 | 23 | 18.056 | 25 |

### 2.2 Code Size Impact

*Source: `code_size` table*

| N | Header Lines | Header Size | Object Size |
|---|--------------|-------------|-------------|
| 2 | 48 | 1.2 KB | 0.6 KB |
| 3 | 57 | 1.4 KB | 0.7 KB |
| 4 | 94 | 2.4 KB | 1.1 KB |
| 5 | 287 | 6.9 KB | 3.2 KB |
| 6 | 97 | 2.4 KB | 1.7 KB |
| 7 | 110 | 2.8 KB | 3.8 KB |
| 8 | 181 | 5.4 KB | 5.0 KB |
| 9 | 322 | 11.9 KB | 7.7 KB |
| 10 | 574 | 25.9 KB | 14.3 KB |

## 3. Algorithmic Counts

Comparing total comparisons and moves between stock std::stable_sort and dtsort-patched version.

### 3.1 Baseline Validation

Validates that dtsort never exceeds stock comparison or move counts across all permutations.

*Source: `v_consistency_summary` view*

#### GCC16 Validation
| Variant | N | Stock Max Cmp | dtsort Max Cmp | Gain | Stock Max Move | dtsort Max Move | Gain | Valid? |
|---------|---|---------------|----------------|------|----------------|-----------------|------|--------|
| dtsort | 2 | 1 | 1 | 0.0% | 3 | 3 | 0.0% | ✅ |
| dtsort | 3 | 4 | 3 | **+33.3%** | 7 | 4 | **+75.0%** | ✅ |
| dtsort | 4 | 7 | 5 | **+40.0%** | 12 | 6 | **+100.0%** | ✅ |
| dtsort | 5 | 11 | 7 | **+57.1%** | 17 | 7 | **+142.9%** | ✅ |
| dtsort | 6 | 15 | 11 | **+36.4%** | 22 | 16 | **+37.5%** | ✅ |
| dtsort | 7 | 19 | 14 | **+35.7%** | 27 | 18 | **+50.0%** | ✅ |
| dtsort | 8 | 25 | 17 | **+47.1%** | 34 | 20 | **+70.0%** | ✅ |
| dtsort | 9 | 26 | 20 | **+30.0%** | 39 | 23 | **+69.6%** | ✅ |
| dtsort | 10 | 34 | 23 | **+47.8%** | 46 | 26 | **+76.9%** | ✅ |

#### LLVM19 Validation
| Variant | N | Stock Max Cmp | dtsort Max Cmp | Gain | Stock Max Move | dtsort Max Move | Gain | Valid? |
|---------|---|---------------|----------------|------|----------------|-----------------|------|--------|
| dtsort | 2 | 1 | 1 | 0.0% | 3 | 3 | 0.0% | ✅ |
| dtsort | 3 | 3 | 3 | 0.0% | 6 | 4 | **+50.0%** | ✅ |
| dtsort | 4 | 5 | 5 | 0.0% | 8 | 6 | **+33.3%** | ✅ |
| dtsort | 5 | 8 | 7 | **+14.3%** | 13 | 7 | **+85.7%** | ✅ |
| dtsort | 6 | 11 | 11 | 0.0% | 18 | 16 | **+12.5%** | ✅ |
| dtsort | 7 | 15 | 14 | **+7.1%** | 23 | 18 | **+27.8%** | ✅ |
| dtsort | 8 | 19 | 17 | **+11.8%** | 26 | 20 | **+30.0%** | ✅ |
| dtsort | 9 | 24 | 20 | **+20.0%** | 30 | 23 | **+30.4%** | ✅ |
| dtsort | 10 | 29 | 23 | **+26.1%** | 35 | 26 | **+34.6%** | ✅ |

#### dt_stable_sort Validation
| Variant | N | Stock Max Cmp | dtsort Max Cmp | Gain | Stock Max Move | dtsort Max Move | Gain | Valid? |
|---------|---|---------------|----------------|------|----------------|-----------------|------|--------|
| dtsort | 2 | 1 | 1 | 0.0% | 3 | 3 | 0.0% | ✅ |
| dtsort | 3 | 3 | 3 | 0.0% | 6 | 4 | **+50.0%** | ✅ |
| dtsort | 4 | 5 | 5 | 0.0% | 8 | 6 | **+33.3%** | ✅ |
| dtsort | 5 | 8 | 7 | **+14.3%** | 13 | 7 | **+85.7%** | ✅ |
| dtsort | 6 | 11 | 11 | 0.0% | 18 | 16 | **+12.5%** | ✅ |
| dtsort | 7 | 15 | 13 | **+15.4%** | 23 | 18 | **+27.8%** | ✅ |
| dtsort | 8 | 19 | 17 | **+11.8%** | 26 | 20 | **+30.0%** | ✅ |
| dtsort | 9 | 24 | 20 | **+20.0%** | 30 | 23 | **+30.4%** | ✅ |
| dtsort | 10 | 29 | 23 | **+26.1%** | 35 | 26 | **+34.6%** | ✅ |

### 3.2 Small N Average Counts

*Source: `v_consistency_summary` view*

#### Comparison Counts
| Impl | Variant | N | Stock Cmp (Avg/Max) | dtsort Cmp (Avg/Max) | Gain (Avg) | Gain (Max) | Valid? |
|------|---------|---|---------------------|----------------------|------------|------------|--------|
| GCC16 | dtsort | 2 | 1.00/1 | 1.00/1 | 0.0% | 0.0% | ✅ |
| GCC16 | dtsort | 3 | 3.15/4 | 2.62/3 | **+20.2%** | **+33.3%** | ✅ |
| GCC16 | dtsort | 4 | 5.79/7 | 4.61/5 | **+25.6%** | **+40.0%** | ✅ |
| GCC16 | dtsort | 5 | 8.41/11 | 6.96/7 | **+20.8%** | **+57.1%** | ✅ |
| GCC16 | dtsort | 6 | 11.55/15 | 9.63/11 | **+19.9%** | **+36.4%** | ✅ |
| GCC16 | dtsort | 7 | 15.00/19 | 12.48/14 | **+20.2%** | **+35.7%** | ✅ |
| GCC16 | dtsort | 8 | 18.60/25 | 15.46/17 | **+20.3%** | **+47.1%** | ✅ |
| GCC16 | dtsort | 9 | 22.45/26 | 18.63/20 | **+20.5%** | **+30.0%** | ✅ |
| GCC16 | dtsort | 10 | 26.64/34 | 21.93/23 | **+21.5%** | **+47.8%** | ✅ |
| LLVM19 | dtsort | 2 | 1.00/1 | 1.00/1 | 0.0% | 0.0% | ✅ |
| LLVM19 | dtsort | 3 | 2.64/3 | 2.62/3 | **+0.8%** | 0.0% | ✅ |
| LLVM19 | dtsort | 4 | 4.63/5 | 4.61/5 | +0.4% | 0.0% | ✅ |
| LLVM19 | dtsort | 5 | 6.95/8 | 6.96/7 | -0.1% | **+14.3%** | ✅ |
| LLVM19 | dtsort | 6 | 9.52/11 | 9.63/11 | -1.1% | 0.0% | ✅ |
| LLVM19 | dtsort | 7 | 12.26/15 | 12.48/14 | -1.8% | **+7.1%** | ✅ |
| LLVM19 | dtsort | 8 | 15.80/19 | 15.46/17 | **+2.2%** | **+11.8%** | ✅ |
| LLVM19 | dtsort | 9 | 19.77/24 | 18.63/20 | **+6.1%** | **+20.0%** | ✅ |
| LLVM19 | dtsort | 10 | 23.17/29 | 21.93/23 | **+5.7%** | **+26.1%** | ✅ |
| dt_stable_sort | dtsort | 2 | 1.00/1 | 1.00/1 | 0.0% | 0.0% | ✅ |
| dt_stable_sort | dtsort | 3 | 2.64/3 | 2.62/3 | **+0.8%** | 0.0% | ✅ |
| dt_stable_sort | dtsort | 4 | 4.63/5 | 4.61/5 | +0.4% | 0.0% | ✅ |
| dt_stable_sort | dtsort | 5 | 6.95/8 | 6.96/7 | -0.1% | **+14.3%** | ✅ |
| dt_stable_sort | dtsort | 6 | 9.52/11 | 9.59/11 | -0.7% | 0.0% | ✅ |
| dt_stable_sort | dtsort | 7 | 12.26/15 | 12.45/13 | -1.5% | **+15.4%** | ✅ |
| dt_stable_sort | dtsort | 8 | 15.80/19 | 15.46/17 | **+2.2%** | **+11.8%** | ✅ |
| dt_stable_sort | dtsort | 9 | 19.77/24 | 18.63/20 | **+6.1%** | **+20.0%** | ✅ |
| dt_stable_sort | dtsort | 10 | 23.17/29 | 21.93/23 | **+5.7%** | **+26.1%** | ✅ |

#### Move Counts
| Impl | Variant | N | Stock Move (Avg/Max) | dtsort Move (Avg/Max) | Gain (Avg) | Gain (Max) | Valid? |
|------|---------|---|----------------------|-----------------------|------------|------------|--------|
| GCC16 | dtsort | 2 | 2.40/3 | 1.20/3 | **+100.0%** | 0.0% | ✅ |
| GCC16 | dtsort | 3 | 5.33/7 | 2.73/4 | **+95.2%** | **+75.0%** | ✅ |
| GCC16 | dtsort | 4 | 9.93/12 | 3.67/6 | **+170.6%** | **+100.0%** | ✅ |
| GCC16 | dtsort | 5 | 13.31/17 | 5.08/7 | **+162.0%** | **+142.9%** | ✅ |
| GCC16 | dtsort | 6 | 18.35/22 | 9.66/16 | **+90.0%** | **+37.5%** | ✅ |
| GCC16 | dtsort | 7 | 22.28/27 | 11.67/18 | **+90.9%** | **+50.0%** | ✅ |
| GCC16 | dtsort | 8 | 28.27/34 | 14.97/20 | **+88.8%** | **+70.0%** | ✅ |
| GCC16 | dtsort | 9 | 33.02/39 | 17.97/23 | **+83.8%** | **+69.6%** | ✅ |
| GCC16 | dtsort | 10 | 39.03/46 | 20.05/26 | **+94.7%** | **+76.9%** | ✅ |
| LLVM19 | dtsort | 2 | 1.20/3 | 1.20/3 | 0.0% | 0.0% | ✅ |
| LLVM19 | dtsort | 3 | 6.00/6 | 2.73/4 | **+119.8%** | **+50.0%** | ✅ |
| LLVM19 | dtsort | 4 | 8.00/8 | 3.67/6 | **+118.0%** | **+33.3%** | ✅ |
| LLVM19 | dtsort | 5 | 11.21/13 | 5.08/7 | **+120.7%** | **+85.7%** | ✅ |
| LLVM19 | dtsort | 6 | 14.52/18 | 9.66/16 | **+50.3%** | **+12.5%** | ✅ |
| LLVM19 | dtsort | 7 | 17.79/23 | 11.67/18 | **+52.4%** | **+27.8%** | ✅ |
| LLVM19 | dtsort | 8 | 21.21/26 | 14.97/20 | **+41.7%** | **+30.0%** | ✅ |
| LLVM19 | dtsort | 9 | 25.47/30 | 17.97/23 | **+41.7%** | **+30.4%** | ✅ |
| LLVM19 | dtsort | 10 | 29.03/35 | 20.05/26 | **+44.8%** | **+34.6%** | ✅ |
| dt_stable_sort | dtsort | 2 | 1.20/3 | 1.20/3 | 0.0% | 0.0% | ✅ |
| dt_stable_sort | dtsort | 3 | 6.00/6 | 2.73/4 | **+119.8%** | **+50.0%** | ✅ |
| dt_stable_sort | dtsort | 4 | 8.00/8 | 3.67/6 | **+118.0%** | **+33.3%** | ✅ |
| dt_stable_sort | dtsort | 5 | 11.21/13 | 5.08/7 | **+120.7%** | **+85.7%** | ✅ |
| dt_stable_sort | dtsort | 6 | 14.52/18 | 9.66/16 | **+50.3%** | **+12.5%** | ✅ |
| dt_stable_sort | dtsort | 7 | 17.79/23 | 11.67/18 | **+52.4%** | **+27.8%** | ✅ |
| dt_stable_sort | dtsort | 8 | 21.21/26 | 14.97/20 | **+41.7%** | **+30.0%** | ✅ |
| dt_stable_sort | dtsort | 9 | 25.47/30 | 17.97/23 | **+41.7%** | **+30.4%** | ✅ |
| dt_stable_sort | dtsort | 10 | 29.03/35 | 20.05/26 | **+44.8%** | **+34.6%** | ✅ |

### 3.3 Large Dataset Metrics

*Source: `v_large_n_summary` view*

#### N=2

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 1.00 | 1.00 | 0.0% | 2.30 | 0.90 | **+155.6%** |
| GCC16 | dtsort | IntNakedReversed | 1.00 | 1.00 | 0.0% | 2.65 | 1.95 | **+35.9%** |
| GCC16 | dtsort | IntNakedSorted | 1.00 | 1.00 | 0.0% | 2.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 1.00 | 1.00 | 0.0% | 2.30 | 0.90 | **+155.6%** |
| GCC16 | dtsort | WideRowReversed | 1.00 | 1.00 | 0.0% | 2.65 | 1.95 | **+35.9%** |
| GCC16 | dtsort | WideRowSorted | 1.00 | 1.00 | 0.0% | 2.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 1.00 | 1.00 | 0.0% | 0.90 | 0.90 | 0.0% |
| LLVM19 | dtsort | IntNakedReversed | 1.00 | 1.00 | 0.0% | 1.95 | 1.95 | 0.0% |
| LLVM19 | dtsort | IntNakedSorted | 1.00 | 1.00 | 0.0% | 0.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 1.00 | 1.00 | 0.0% | 0.90 | 0.90 | 0.0% |
| LLVM19 | dtsort | WideRowReversed | 1.00 | 1.00 | 0.0% | 1.95 | 1.95 | 0.0% |
| LLVM19 | dtsort | WideRowSorted | 1.00 | 1.00 | 0.0% | 0.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 1.00 | 1.00 | 0.0% | 0.90 | 0.90 | 0.0% |
| dt_stable_sort | dtsort-em | IntNaked | 1.00 | 1.00 | 0.0% | 0.90 | 0.90 | 0.0% |
| dt_stable_sort | dtsort-am | IntNakedReversed | 1.00 | 1.00 | 0.0% | 1.95 | 1.95 | 0.0% |
| dt_stable_sort | dtsort-em | IntNakedReversed | 1.00 | 1.00 | 0.0% | 1.95 | 1.95 | 0.0% |
| dt_stable_sort | dtsort-am | IntNakedSorted | 1.00 | 1.00 | 0.0% | 0.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 1.00 | 1.00 | 0.0% | 0.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 1.00 | 1.00 | 0.0% | 0.90 | 0.90 | 0.0% |
| dt_stable_sort | dtsort-em | WideRow | 1.00 | 1.00 | 0.0% | 0.90 | 0.90 | 0.0% |
| dt_stable_sort | dtsort-am | WideRowReversed | 1.00 | 1.00 | 0.0% | 1.95 | 1.95 | 0.0% |
| dt_stable_sort | dtsort-em | WideRowReversed | 1.00 | 1.00 | 0.0% | 1.95 | 1.95 | 0.0% |
| dt_stable_sort | dtsort-am | WideRowSorted | 1.00 | 1.00 | 0.0% | 0.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 1.00 | 1.00 | 0.0% | 0.00 | 0.00 | — |

#### N=3

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 3.35 | 2.60 | **+28.8%** | 5.35 | 2.95 | **+81.4%** |
| GCC16 | dtsort | IntNakedReversed | 3.40 | 2.60 | **+30.8%** | 6.25 | 3.45 | **+81.2%** |
| GCC16 | dtsort | IntNakedSorted | 3.00 | 3.00 | 0.0% | 4.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 3.35 | 2.60 | **+28.8%** | 5.35 | 2.95 | **+81.4%** |
| GCC16 | dtsort | WideRowReversed | 3.40 | 2.60 | **+30.8%** | 6.25 | 3.45 | **+81.2%** |
| GCC16 | dtsort | WideRowSorted | 3.00 | 3.00 | 0.0% | 4.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 2.60 | 2.60 | 0.0% | 6.00 | 2.95 | **+103.4%** |
| LLVM19 | dtsort | IntNakedReversed | 2.95 | 2.60 | **+13.5%** | 6.00 | 3.45 | **+73.9%** |
| LLVM19 | dtsort | IntNakedSorted | 2.00 | 3.00 | -33.3% | 6.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 2.60 | 2.60 | 0.0% | 6.00 | 2.95 | **+103.4%** |
| LLVM19 | dtsort | WideRowReversed | 2.95 | 2.60 | **+13.5%** | 6.00 | 3.45 | **+73.9%** |
| LLVM19 | dtsort | WideRowSorted | 2.00 | 3.00 | -33.3% | 6.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 2.60 | 2.60 | 0.0% | 6.00 | 2.95 | **+103.4%** |
| dt_stable_sort | dtsort-em | IntNaked | 2.60 | 2.60 | 0.0% | 6.00 | 2.95 | **+103.4%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 2.95 | 2.60 | **+13.5%** | 6.00 | 3.45 | **+73.9%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 2.95 | 2.60 | **+13.5%** | 6.00 | 3.45 | **+73.9%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 2.00 | 3.00 | -33.3% | 6.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 2.00 | 3.00 | -33.3% | 6.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 2.60 | 2.60 | 0.0% | 6.00 | 2.95 | **+103.4%** |
| dt_stable_sort | dtsort-em | WideRow | 2.60 | 2.60 | 0.0% | 6.00 | 2.95 | **+103.4%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 2.95 | 2.60 | **+13.5%** | 6.00 | 3.45 | **+73.9%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 2.95 | 2.60 | **+13.5%** | 6.00 | 3.45 | **+73.9%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 2.00 | 3.00 | -33.3% | 6.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 2.00 | 3.00 | -33.3% | 6.00 | 0.00 | — |

#### N=4

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 6.05 | 4.70 | **+28.7%** | 10.10 | 4.05 | **+149.4%** |
| GCC16 | dtsort | IntNakedReversed | 5.10 | 4.40 | **+15.9%** | 11.10 | 4.55 | **+144.0%** |
| GCC16 | dtsort | IntNakedSorted | 6.00 | 4.00 | **+50.0%** | 8.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 6.05 | 4.70 | **+28.7%** | 10.10 | 4.05 | **+149.4%** |
| GCC16 | dtsort | WideRowReversed | 5.10 | 4.40 | **+15.9%** | 11.10 | 4.55 | **+144.0%** |
| GCC16 | dtsort | WideRowSorted | 6.00 | 4.00 | **+50.0%** | 8.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 4.85 | 4.70 | **+3.2%** | 8.00 | 4.05 | **+97.5%** |
| LLVM19 | dtsort | IntNakedReversed | 4.30 | 4.40 | -2.3% | 8.00 | 4.55 | **+75.8%** |
| LLVM19 | dtsort | IntNakedSorted | 4.00 | 4.00 | 0.0% | 8.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 4.85 | 4.70 | **+3.2%** | 8.00 | 4.05 | **+97.5%** |
| LLVM19 | dtsort | WideRowReversed | 4.30 | 4.40 | -2.3% | 8.00 | 4.55 | **+75.8%** |
| LLVM19 | dtsort | WideRowSorted | 4.00 | 4.00 | 0.0% | 8.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 4.85 | 4.70 | **+3.2%** | 8.00 | 4.05 | **+97.5%** |
| dt_stable_sort | dtsort-em | IntNaked | 4.85 | 4.70 | **+3.2%** | 8.00 | 4.05 | **+97.5%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 4.30 | 4.40 | -2.3% | 8.00 | 4.55 | **+75.8%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 4.30 | 4.40 | -2.3% | 8.00 | 4.55 | **+75.8%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 4.00 | 4.00 | 0.0% | 8.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 4.00 | 4.00 | 0.0% | 8.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 4.85 | 4.70 | **+3.2%** | 8.00 | 4.05 | **+97.5%** |
| dt_stable_sort | dtsort-em | WideRow | 4.85 | 4.70 | **+3.2%** | 8.00 | 4.05 | **+97.5%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 4.30 | 4.40 | -2.3% | 8.00 | 4.55 | **+75.8%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 4.30 | 4.40 | -2.3% | 8.00 | 4.55 | **+75.8%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 4.00 | 4.00 | 0.0% | 8.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 4.00 | 4.00 | 0.0% | 8.00 | 0.00 | — |

#### N=5

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 8.65 | 7.00 | **+23.6%** | 13.00 | 5.00 | **+160.0%** |
| GCC16 | dtsort | IntNakedReversed | 7.80 | 6.60 | **+18.2%** | 15.75 | 6.10 | **+158.2%** |
| GCC16 | dtsort | IntNakedSorted | 8.00 | 7.00 | **+14.3%** | 10.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 8.65 | 7.00 | **+23.6%** | 13.00 | 5.00 | **+160.0%** |
| GCC16 | dtsort | WideRowReversed | 7.80 | 6.60 | **+18.2%** | 15.75 | 6.10 | **+158.2%** |
| GCC16 | dtsort | WideRowSorted | 8.00 | 7.00 | **+14.3%** | 10.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 6.80 | 7.00 | -2.9% | 11.25 | 5.00 | **+125.0%** |
| LLVM19 | dtsort | IntNakedReversed | 7.25 | 6.60 | **+9.8%** | 12.10 | 6.10 | **+98.4%** |
| LLVM19 | dtsort | IntNakedSorted | 5.00 | 7.00 | -28.6% | 10.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 6.80 | 7.00 | -2.9% | 11.25 | 5.00 | **+125.0%** |
| LLVM19 | dtsort | WideRowReversed | 7.25 | 6.60 | **+9.8%** | 12.10 | 6.10 | **+98.4%** |
| LLVM19 | dtsort | WideRowSorted | 5.00 | 7.00 | -28.6% | 10.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 6.80 | 7.00 | -2.9% | 11.25 | 5.00 | **+125.0%** |
| dt_stable_sort | dtsort-em | IntNaked | 6.80 | 7.00 | -2.9% | 11.25 | 5.00 | **+125.0%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 7.25 | 6.60 | **+9.8%** | 12.10 | 6.10 | **+98.4%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 7.25 | 6.60 | **+9.8%** | 12.10 | 6.10 | **+98.4%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 5.00 | 7.00 | -28.6% | 10.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 5.00 | 7.00 | -28.6% | 10.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 6.80 | 7.00 | -2.9% | 11.25 | 5.00 | **+125.0%** |
| dt_stable_sort | dtsort-em | WideRow | 6.80 | 7.00 | -2.9% | 11.25 | 5.00 | **+125.0%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 7.25 | 6.60 | **+9.8%** | 12.10 | 6.10 | **+98.4%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 7.25 | 6.60 | **+9.8%** | 12.10 | 6.10 | **+98.4%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 5.00 | 7.00 | -28.6% | 10.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 5.00 | 7.00 | -28.6% | 10.00 | 0.00 | — |

#### N=6

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 11.75 | 9.55 | **+23.0%** | 18.05 | 9.75 | **+85.1%** |
| GCC16 | dtsort | IntNakedReversed | 9.45 | 9.40 | **+0.5%** | 21.30 | 14.00 | **+52.1%** |
| GCC16 | dtsort | IntNakedSorted | 11.00 | 6.00 | **+83.3%** | 14.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 11.75 | 9.55 | **+23.0%** | 18.05 | 9.75 | **+85.1%** |
| GCC16 | dtsort | WideRowReversed | 9.45 | 9.40 | **+0.5%** | 21.30 | 14.00 | **+52.1%** |
| GCC16 | dtsort | WideRowSorted | 11.00 | 6.00 | **+83.3%** | 14.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 9.25 | 9.55 | -3.1% | 13.95 | 9.75 | **+43.1%** |
| LLVM19 | dtsort | IntNakedReversed | 9.40 | 9.40 | 0.0% | 16.40 | 14.00 | **+17.1%** |
| LLVM19 | dtsort | IntNakedSorted | 7.00 | 6.00 | **+16.7%** | 12.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 9.25 | 9.55 | -3.1% | 13.95 | 9.75 | **+43.1%** |
| LLVM19 | dtsort | WideRowReversed | 9.40 | 9.40 | 0.0% | 16.40 | 14.00 | **+17.1%** |
| LLVM19 | dtsort | WideRowSorted | 7.00 | 6.00 | **+16.7%** | 12.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 9.25 | 9.55 | -3.1% | 13.95 | 9.75 | **+43.1%** |
| dt_stable_sort | dtsort-em | IntNaked | 9.25 | 9.55 | -3.1% | 13.95 | 9.75 | **+43.1%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 9.40 | 9.40 | 0.0% | 16.40 | 14.00 | **+17.1%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 9.40 | 9.35 | **+0.5%** | 16.40 | 14.00 | **+17.1%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 7.00 | 6.00 | **+16.7%** | 12.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 7.00 | 8.00 | -12.5% | 12.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 9.25 | 9.55 | -3.1% | 13.95 | 9.75 | **+43.1%** |
| dt_stable_sort | dtsort-em | WideRow | 9.25 | 9.55 | -3.1% | 13.95 | 9.75 | **+43.1%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 9.40 | 9.40 | 0.0% | 16.40 | 14.00 | **+17.1%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 9.40 | 9.35 | **+0.5%** | 16.40 | 14.00 | **+17.1%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 7.00 | 6.00 | **+16.7%** | 12.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 7.00 | 8.00 | -12.5% | 12.00 | 0.00 | — |

#### N=7

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 15.60 | 12.35 | **+26.3%** | 22.35 | 12.45 | **+79.5%** |
| GCC16 | dtsort | IntNakedReversed | 12.60 | 12.20 | **+3.3%** | 27.10 | 16.50 | **+64.2%** |
| GCC16 | dtsort | IntNakedSorted | 13.00 | 9.00 | **+44.4%** | 16.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 15.60 | 12.35 | **+26.3%** | 22.35 | 12.45 | **+79.5%** |
| GCC16 | dtsort | WideRowReversed | 12.60 | 12.20 | **+3.3%** | 27.10 | 16.50 | **+64.2%** |
| GCC16 | dtsort | WideRowSorted | 13.00 | 9.00 | **+44.4%** | 16.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 12.55 | 12.35 | **+1.6%** | 17.45 | 12.45 | **+40.2%** |
| LLVM19 | dtsort | IntNakedReversed | 13.20 | 12.20 | **+8.2%** | 20.85 | 16.50 | **+26.4%** |
| LLVM19 | dtsort | IntNakedSorted | 8.00 | 9.00 | -11.1% | 14.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 12.55 | 12.35 | **+1.6%** | 17.45 | 12.45 | **+40.2%** |
| LLVM19 | dtsort | WideRowReversed | 13.20 | 12.20 | **+8.2%** | 20.85 | 16.50 | **+26.4%** |
| LLVM19 | dtsort | WideRowSorted | 8.00 | 9.00 | -11.1% | 14.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 12.55 | 12.35 | **+1.6%** | 17.45 | 12.45 | **+40.2%** |
| dt_stable_sort | dtsort-em | IntNaked | 12.55 | 12.35 | **+1.6%** | 17.45 | 12.45 | **+40.2%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 13.20 | 12.20 | **+8.2%** | 20.85 | 16.50 | **+26.4%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 13.20 | 12.20 | **+8.2%** | 20.85 | 16.50 | **+26.4%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 8.00 | 9.00 | -11.1% | 14.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 8.00 | 12.00 | -33.3% | 14.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 12.55 | 12.35 | **+1.6%** | 17.45 | 12.45 | **+40.2%** |
| dt_stable_sort | dtsort-em | WideRow | 12.55 | 12.35 | **+1.6%** | 17.45 | 12.45 | **+40.2%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 13.20 | 12.20 | **+8.2%** | 20.85 | 16.50 | **+26.4%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 13.20 | 12.20 | **+8.2%** | 20.85 | 16.50 | **+26.4%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 8.00 | 9.00 | -11.1% | 14.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 8.00 | 12.00 | -33.3% | 14.00 | 0.00 | — |

#### N=8

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 17.75 | 15.40 | **+15.3%** | 28.05 | 15.15 | **+85.1%** |
| GCC16 | dtsort | IntNakedReversed | 15.05 | 14.95 | **+0.7%** | 33.05 | 18.50 | **+78.6%** |
| GCC16 | dtsort | IntNakedSorted | 16.00 | 11.00 | **+45.5%** | 20.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 17.75 | 15.40 | **+15.3%** | 28.05 | 15.15 | **+85.1%** |
| GCC16 | dtsort | WideRowReversed | 15.05 | 14.95 | **+0.7%** | 33.05 | 18.50 | **+78.6%** |
| GCC16 | dtsort | WideRowSorted | 16.00 | 11.00 | **+45.5%** | 20.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 15.35 | 15.40 | -0.3% | 21.15 | 15.15 | **+39.6%** |
| LLVM19 | dtsort | IntNakedReversed | 15.85 | 14.95 | **+6.0%** | 25.05 | 18.50 | **+35.4%** |
| LLVM19 | dtsort | IntNakedSorted | 10.00 | 11.00 | -9.1% | 16.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 15.35 | 15.40 | -0.3% | 21.15 | 15.15 | **+39.6%** |
| LLVM19 | dtsort | WideRowReversed | 15.85 | 14.95 | **+6.0%** | 25.05 | 18.50 | **+35.4%** |
| LLVM19 | dtsort | WideRowSorted | 10.00 | 11.00 | -9.1% | 16.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 15.35 | 15.40 | -0.3% | 21.15 | 15.15 | **+39.6%** |
| dt_stable_sort | dtsort-em | IntNaked | 15.35 | 15.40 | -0.3% | 21.15 | 15.15 | **+39.6%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 15.85 | 14.95 | **+6.0%** | 25.05 | 18.50 | **+35.4%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 15.85 | 14.95 | **+6.0%** | 25.05 | 18.50 | **+35.4%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 10.00 | 11.00 | -9.1% | 16.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 10.00 | 16.00 | -37.5% | 16.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 15.35 | 15.40 | -0.3% | 21.15 | 15.15 | **+39.6%** |
| dt_stable_sort | dtsort-em | WideRow | 15.35 | 15.40 | -0.3% | 21.15 | 15.15 | **+39.6%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 15.85 | 14.95 | **+6.0%** | 25.05 | 18.50 | **+35.4%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 15.85 | 14.95 | **+6.0%** | 25.05 | 18.50 | **+35.4%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 10.00 | 11.00 | -9.1% | 16.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 10.00 | 16.00 | -37.5% | 16.00 | 0.00 | — |

#### N=9

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 22.15 | 18.60 | **+19.1%** | 33.20 | 18.35 | **+80.9%** |
| GCC16 | dtsort | IntNakedReversed | 18.25 | 17.55 | **+4.0%** | 39.45 | 21.05 | **+87.4%** |
| GCC16 | dtsort | IntNakedSorted | 18.00 | 12.00 | **+50.0%** | 22.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 22.15 | 18.60 | **+19.1%** | 33.20 | 18.35 | **+80.9%** |
| GCC16 | dtsort | WideRowReversed | 18.25 | 17.55 | **+4.0%** | 39.45 | 21.05 | **+87.4%** |
| GCC16 | dtsort | WideRowSorted | 18.00 | 12.00 | **+50.0%** | 22.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 19.45 | 18.60 | **+4.6%** | 25.25 | 18.35 | **+37.6%** |
| LLVM19 | dtsort | IntNakedReversed | 20.95 | 17.55 | **+19.4%** | 30.70 | 21.05 | **+45.8%** |
| LLVM19 | dtsort | IntNakedSorted | 11.00 | 12.00 | -8.3% | 18.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 19.45 | 18.60 | **+4.6%** | 25.25 | 18.35 | **+37.6%** |
| LLVM19 | dtsort | WideRowReversed | 20.95 | 17.55 | **+19.4%** | 30.70 | 21.05 | **+45.8%** |
| LLVM19 | dtsort | WideRowSorted | 11.00 | 12.00 | -8.3% | 18.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 19.45 | 18.60 | **+4.6%** | 25.25 | 18.35 | **+37.6%** |
| dt_stable_sort | dtsort-em | IntNaked | 19.45 | 18.60 | **+4.6%** | 25.25 | 18.35 | **+37.6%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 20.95 | 17.55 | **+19.4%** | 30.70 | 21.05 | **+45.8%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 20.95 | 17.55 | **+19.4%** | 30.70 | 21.05 | **+45.8%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 11.00 | 12.00 | -8.3% | 18.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 11.00 | 18.00 | -38.9% | 18.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 19.45 | 18.60 | **+4.6%** | 25.25 | 18.35 | **+37.6%** |
| dt_stable_sort | dtsort-em | WideRow | 19.45 | 18.60 | **+4.6%** | 25.25 | 18.35 | **+37.6%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 20.95 | 17.55 | **+19.4%** | 30.70 | 21.05 | **+45.8%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 20.95 | 17.55 | **+19.4%** | 30.70 | 21.05 | **+45.8%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 11.00 | 12.00 | -8.3% | 18.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 11.00 | 18.00 | -38.9% | 18.00 | 0.00 | — |

#### N=10

| Impl | Variant | Type | Stock Avg Cmp | dtsort Avg Cmp | Gain Cmp | Stock Avg Move | dtsort Avg Move | Gain Move |
|------|---------|------|---------------|----------------|----------|----------------|-----------------|-----------|
| GCC16 | dtsort | IntNaked | 25.65 | 21.90 | **+17.1%** | 38.40 | 19.85 | **+93.5%** |
| GCC16 | dtsort | IntNakedReversed | 20.35 | 20.90 | -2.6% | 47.30 | 26.70 | **+77.2%** |
| GCC16 | dtsort | IntNakedSorted | 21.00 | 15.00 | **+40.0%** | 26.00 | 0.00 | — |
| GCC16 | dtsort | WideRow | 25.65 | 21.90 | **+17.1%** | 38.40 | 19.85 | **+93.5%** |
| GCC16 | dtsort | WideRowReversed | 20.35 | 20.90 | -2.6% | 47.30 | 26.70 | **+77.2%** |
| GCC16 | dtsort | WideRowSorted | 21.00 | 15.00 | **+40.0%** | 26.00 | 0.00 | — |
| LLVM19 | dtsort | IntNaked | 22.40 | 21.90 | **+2.3%** | 28.45 | 19.85 | **+43.3%** |
| LLVM19 | dtsort | IntNakedReversed | 24.85 | 20.90 | **+18.9%** | 36.30 | 26.70 | **+36.0%** |
| LLVM19 | dtsort | IntNakedSorted | 13.00 | 15.00 | -13.3% | 20.00 | 0.00 | — |
| LLVM19 | dtsort | WideRow | 22.40 | 21.90 | **+2.3%** | 28.45 | 19.85 | **+43.3%** |
| LLVM19 | dtsort | WideRowReversed | 24.85 | 20.90 | **+18.9%** | 36.30 | 26.70 | **+36.0%** |
| LLVM19 | dtsort | WideRowSorted | 13.00 | 15.00 | -13.3% | 20.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | IntNaked | 22.40 | 21.90 | **+2.3%** | 28.45 | 19.85 | **+43.3%** |
| dt_stable_sort | dtsort-em | IntNaked | 22.40 | 21.90 | **+2.3%** | 28.45 | 19.85 | **+43.3%** |
| dt_stable_sort | dtsort-am | IntNakedReversed | 24.85 | 20.90 | **+18.9%** | 36.30 | 26.70 | **+36.0%** |
| dt_stable_sort | dtsort-em | IntNakedReversed | 24.85 | 20.90 | **+18.9%** | 36.30 | 26.70 | **+36.0%** |
| dt_stable_sort | dtsort-am | IntNakedSorted | 13.00 | 15.00 | -13.3% | 20.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | IntNakedSorted | 13.00 | 21.00 | -38.1% | 20.00 | 0.00 | — |
| dt_stable_sort | dtsort-am | WideRow | 22.40 | 21.90 | **+2.3%** | 28.45 | 19.85 | **+43.3%** |
| dt_stable_sort | dtsort-em | WideRow | 22.40 | 21.90 | **+2.3%** | 28.45 | 19.85 | **+43.3%** |
| dt_stable_sort | dtsort-am | WideRowReversed | 24.85 | 20.90 | **+18.9%** | 36.30 | 26.70 | **+36.0%** |
| dt_stable_sort | dtsort-em | WideRowReversed | 24.85 | 20.90 | **+18.9%** | 36.30 | 26.70 | **+36.0%** |
| dt_stable_sort | dtsort-am | WideRowSorted | 13.00 | 15.00 | -13.3% | 20.00 | 0.00 | — |
| dt_stable_sort | dtsort-em | WideRowSorted | 13.00 | 21.00 | -38.1% | 20.00 | 0.00 | — |

## 4. Wallclock Measurements

### 4.1 Throughput Analysis

*Source: `v_wall_clock_summary` view*

#### GCC16 (IntNaked)
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 16.12 | 2.97 | **+443.1%** | ✅ |
| 3 | dtsort | 17.48 | 3.46 | **+405.1%** | ✅ |
| 4 | dtsort | 19.27 | 4.21 | **+357.7%** | ✅ |
| 5 | dtsort | 21.99 | 5.00 | **+339.7%** | ✅ |
| 6 | dtsort | 24.35 | 8.72 | **+179.3%** | ✅ |
| 7 | dtsort | 24.82 | 10.82 | **+129.4%** | ✅ |
| 8 | dtsort | 28.76 | 12.38 | **+132.3%** | ✅ |
| 9 | dtsort | 30.22 | 16.42 | **+84.0%** | ✅ |
| 10 | dtsort | 33.11 | 12.98 | **+155.1%** | ✅ |
| 20 | dtsort | 68.31 | 66.93 | **+2.1%** | ✅ |
| 22 | dtsort | 79.75 | 82.22 | -3.0% | ⚠️ |
| 25 | dtsort | 77.00 | 70.91 | **+8.6%** | ✅ |
| 28 | dtsort | 109.79 | 110.48 | -0.6% | ✅ |
| 32 | dtsort | 136.28 | 136.96 | -0.5% | ✅ |
| 36 | dtsort | 147.72 | 146.28 | **+1.0%** | ✅ |
| 40 | dtsort | 170.46 | 175.79 | -3.0% | ⚠️ |
| 45 | dtsort | 168.53 | 255.76 | -34.1% | ⚠️ |
| 51 | dtsort | 191.47 | 201.12 | -4.8% | ⚠️ |
| 57 | dtsort | 248.17 | 256.80 | -3.4% | ⚠️ |
| 64 | dtsort | 374.56 | 380.82 | -1.6% | ⚠️ |
| 72 | dtsort | 441.08 | 453.27 | -2.7% | ⚠️ |
| 81 | dtsort | 403.86 | 421.90 | -4.3% | ⚠️ |
| 91 | dtsort | 473.12 | 487.44 | -2.9% | ⚠️ |
| 102 | dtsort | 651.94 | 667.16 | -2.3% | ⚠️ |
| 114 | dtsort | 777.93 | 807.02 | -3.6% | ⚠️ |
| 129 | dtsort | 824.30 | 846.71 | -2.6% | ⚠️ |
| 144 | dtsort | 1115.27 | 1149.89 | -3.0% | ⚠️ |
| 162 | dtsort | 1229.37 | 1268.86 | -3.1% | ⚠️ |
| 182 | dtsort | 1431.80 | 1486.76 | -3.7% | ⚠️ |
| 205 | dtsort | 1398.09 | 1416.26 | -1.3% | ⚠️ |
| 230 | dtsort | 2221.66 | 2282.85 | -2.7% | ⚠️ |
| 258 | dtsort | 2492.28 | 2549.53 | -2.2% | ⚠️ |
| 290 | dtsort | 2784.75 | 2867.62 | -2.9% | ⚠️ |
| 326 | dtsort | 3139.04 | 3204.37 | -2.0% | ⚠️ |
| 366 | dtsort | 3609.64 | 3723.76 | -3.1% | ⚠️ |
| 411 | dtsort | 3722.04 | 3676.85 | **+1.2%** | ✅ |
| 462 | dtsort | 5379.54 | 5441.63 | -1.1% | ⚠️ |
| 519 | dtsort | 5414.86 | 5399.90 | +0.3% | ✅ |
| 583 | dtsort | 6070.01 | 6067.23 | +0.0% | ✅ |
| 655 | dtsort | 6655.40 | 6776.25 | -1.8% | ⚠️ |
| 736 | dtsort | 8679.05 | 8808.53 | -1.5% | ⚠️ |
| 826 | dtsort | 9893.80 | 9917.08 | -0.2% | ✅ |
| 928 | dtsort | 12707.55 | 12775.93 | -0.5% | ✅ |
| 1,043 | dtsort | 12965.92 | 13042.51 | -0.6% | ✅ |
| 1,171 | dtsort | 14564.43 | 14609.98 | -0.3% | ✅ |
| 1,316 | dtsort | 17591.61 | 17862.12 | -1.5% | ⚠️ |
| 1,478 | dtsort | 20597.23 | 20827.91 | -1.1% | ⚠️ |
| 1,660 | dtsort | 23132.32 | 23324.28 | -0.8% | ✅ |
| 1,865 | dtsort | 26963.98 | 27166.99 | -0.7% | ✅ |
| 2,095 | dtsort | 29954.49 | 30020.38 | -0.2% | ✅ |
| 2,354 | dtsort | 35910.01 | 36377.05 | -1.3% | ⚠️ |
| 2,644 | dtsort | 40902.98 | 41100.21 | -0.5% | ✅ |
| 2,970 | dtsort | 46737.74 | 47104.46 | -0.8% | ✅ |
| 3,336 | dtsort | 53128.48 | 53286.95 | -0.3% | ✅ |
| 3,748 | dtsort | 65660.95 | 66073.58 | -0.6% | ✅ |
| 4,210 | dtsort | 72910.91 | 73420.37 | -0.7% | ✅ |
| 4,729 | dtsort | 77538.97 | 77142.26 | **+0.5%** | ✅ |
| 5,312 | dtsort | 92546.80 | 92844.56 | -0.3% | ✅ |
| 5,967 | dtsort | 100596.01 | 100171.61 | +0.4% | ✅ |
| 6,703 | dtsort | 113194.47 | 112883.92 | +0.3% | ✅ |
| 7,530 | dtsort | 145981.04 | 146663.64 | -0.5% | ✅ |
| 8,458 | dtsort | 163009.43 | 163193.35 | -0.1% | ✅ |
| 9,502 | dtsort | 183133.50 | 184141.23 | -0.5% | ✅ |
| 10,673 | dtsort | 199281.34 | 195064.79 | **+2.2%** | ✅ |
| 11,990 | dtsort | 236855.13 | 237823.97 | -0.4% | ✅ |
| 13,468 | dtsort | 271747.27 | 270567.27 | +0.4% | ✅ |
| 15,129 | dtsort | 312394.20 | 310703.87 | **+0.5%** | ✅ |
| 16,995 | dtsort | 357810.70 | 349491.62 | **+2.4%** | ✅ |
| 19,091 | dtsort | 410176.24 | 401533.84 | **+2.2%** | ✅ |
| 21,445 | dtsort | 462565.46 | 450860.79 | **+2.6%** | ✅ |
| 24,090 | dtsort | 545328.27 | 537498.28 | **+1.5%** | ✅ |
| 27,061 | dtsort | 635271.54 | 588329.09 | **+8.0%** | ✅ |
| 30,398 | dtsort | 747325.14 | 726057.67 | **+2.9%** | ✅ |
| 34,147 | dtsort | 837481.48 | 805443.50 | **+4.0%** | ✅ |
| 38,358 | dtsort | 933915.86 | 906791.99 | **+3.0%** | ✅ |
| 43,089 | dtsort | 1101234.48 | 1055270.16 | **+4.4%** | ✅ |
| 48,403 | dtsort | 1240062.91 | 1186900.10 | **+4.5%** | ✅ |
| 54,372 | dtsort | 1380329.12 | 1347253.73 | **+2.5%** | ✅ |
| 61,077 | dtsort | 1702646.86 | 1650785.90 | **+3.1%** | ✅ |
| 68,609 | dtsort | 1921886.82 | 1876162.81 | **+2.4%** | ✅ |
| 77,071 | dtsort | 2173462.64 | 2122288.98 | **+2.4%** | ✅ |
| 86,575 | dtsort | 2540694.49 | 2471859.93 | **+2.8%** | ✅ |
| 97,252 | dtsort | 2730033.66 | 2695912.15 | **+1.3%** | ✅ |
| 109,246 | dtsort | 3108643.04 | 3085910.93 | **+0.7%** | ✅ |
| 122,718 | dtsort | 3672247.37 | 3662253.51 | +0.3% | ✅ |
| 137,852 | dtsort | 4118847.38 | 4104010.05 | +0.4% | ✅ |
| 154,853 | dtsort | 4816900.29 | 4766345.80 | **+1.1%** | ✅ |
| 173,950 | dtsort | 5324826.97 | 5305783.72 | +0.4% | ✅ |
| 195,402 | dtsort | 5959033.48 | 5945895.30 | +0.2% | ✅ |
| 219,500 | dtsort | 6744418.27 | 6740267.63 | +0.1% | ✅ |
| 246,569 | dtsort | 8178891.96 | 8127515.51 | **+0.6%** | ✅ |
| 276,977 | dtsort | 9140953.41 | 9079334.41 | **+0.7%** | ✅ |
| 311,135 | dtsort | 10273800.25 | 10213586.96 | **+0.6%** | ✅ |
| 349,506 | dtsort | 11433262.97 | 11423795.08 | +0.1% | ✅ |
| 392,608 | dtsort | 12826522.72 | 12813619.69 | +0.1% | ✅ |
| 441,026 | dtsort | 14487092.02 | 14486865.46 | +0.0% | ✅ |
| 495,415 | dtsort | 17312579.18 | 17230588.41 | +0.5% | ✅ |
| 556,512 | dtsort | 18877327.70 | 18896970.70 | -0.1% | ✅ |
| 625,143 | dtsort | 21745040.34 | 21654174.50 | +0.4% | ✅ |
| 702,238 | dtsort | 24205853.45 | 24212895.10 | -0.0% | ✅ |
| 788,841 | dtsort | 27736035.00 | 27625890.00 | +0.4% | ✅ |
| 886,124 | dtsort | 30605416.64 | 30596931.17 | +0.0% | ✅ |
| 995,405 | dtsort | 36645723.69 | 36535925.43 | +0.3% | ✅ |
| 1,118,162 | dtsort | 40098482.88 | 40106303.94 | -0.0% | ✅ |
| 1,256,058 | dtsort | 45120063.94 | 45102104.20 | +0.0% | ✅ |
| 1,410,960 | dtsort | 51331151.79 | 51305071.43 | +0.1% | ✅ |
| 1,584,966 | dtsort | 57532972.17 | 57538819.42 | -0.0% | ✅ |
| 1,780,430 | dtsort | 65052742.43 | 65006246.18 | +0.1% | ✅ |
| 2,000,000 | dtsort | 75485990.77 | 75496537.02 | -0.0% | ✅ |

#### GCC16 (IntNaked (Reverse))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 15.87 | 2.96 | **+437.0%** | ✅ |
| 3 | dtsort | 17.55 | 3.44 | **+410.1%** | ✅ |
| 4 | dtsort | 19.33 | 4.36 | **+343.5%** | ✅ |
| 5 | dtsort | 21.45 | 4.64 | **+362.4%** | ✅ |
| 6 | dtsort | 24.17 | 9.05 | **+167.1%** | ✅ |
| 7 | dtsort | 25.12 | 10.11 | **+148.6%** | ✅ |
| 8 | dtsort | 28.59 | 12.36 | **+131.3%** | ✅ |
| 9 | dtsort | 30.40 | 12.19 | **+149.5%** | ✅ |
| 10 | dtsort | 33.57 | 11.66 | **+187.9%** | ✅ |
| 20 | dtsort | 67.97 | 61.08 | **+11.3%** | ✅ |
| 22 | dtsort | 69.16 | 67.51 | **+2.4%** | ✅ |
| 25 | dtsort | 75.94 | 69.49 | **+9.3%** | ✅ |
| 28 | dtsort | 93.93 | 88.29 | **+6.4%** | ✅ |
| 32 | dtsort | 99.60 | 96.80 | **+2.9%** | ✅ |
| 36 | dtsort | 110.69 | 109.69 | **+0.9%** | ✅ |
| 40 | dtsort | 131.20 | 120.46 | **+8.9%** | ✅ |
| 45 | dtsort | 133.42 | 180.48 | -26.1% | ⚠️ |
| 51 | dtsort | 146.76 | 135.08 | **+8.6%** | ✅ |
| 57 | dtsort | 187.59 | 188.54 | -0.5% | ✅ |
| 64 | dtsort | 250.58 | 256.90 | -2.5% | ⚠️ |
| 72 | dtsort | 303.53 | 304.51 | -0.3% | ✅ |
| 81 | dtsort | 299.66 | 298.10 | **+0.5%** | ✅ |
| 91 | dtsort | 342.81 | 344.56 | -0.5% | ✅ |
| 102 | dtsort | 455.30 | 449.59 | **+1.3%** | ✅ |
| 114 | dtsort | 518.47 | 507.57 | **+2.1%** | ✅ |
| 129 | dtsort | 519.77 | 512.65 | **+1.4%** | ✅ |
| 144 | dtsort | 674.76 | 661.16 | **+2.1%** | ✅ |
| 162 | dtsort | 768.85 | 756.01 | **+1.7%** | ✅ |
| 182 | dtsort | 850.53 | 828.60 | **+2.6%** | ✅ |
| 205 | dtsort | 895.74 | 875.30 | **+2.3%** | ✅ |
| 230 | dtsort | 1214.25 | 1173.94 | **+3.4%** | ✅ |
| 258 | dtsort | 1342.55 | 1315.89 | **+2.0%** | ✅ |
| 290 | dtsort | 1520.15 | 1486.35 | **+2.3%** | ✅ |
| 326 | dtsort | 1727.08 | 1720.41 | +0.4% | ✅ |
| 366 | dtsort | 1991.20 | 1984.82 | +0.3% | ✅ |
| 411 | dtsort | 2197.81 | 2141.42 | **+2.6%** | ✅ |
| 462 | dtsort | 2801.83 | 2787.42 | **+0.5%** | ✅ |
| 519 | dtsort | 2804.32 | 2748.93 | **+2.0%** | ✅ |
| 583 | dtsort | 3245.58 | 3189.43 | **+1.8%** | ✅ |
| 655 | dtsort | 3751.73 | 3661.62 | **+2.5%** | ✅ |
| 736 | dtsort | 4684.45 | 4691.56 | -0.2% | ✅ |
| 826 | dtsort | 5216.69 | 5242.26 | -0.5% | ✅ |
| 928 | dtsort | 6486.72 | 6451.31 | **+0.5%** | ✅ |
| 1,043 | dtsort | 6750.54 | 6671.44 | **+1.2%** | ✅ |
| 1,171 | dtsort | 7765.66 | 7669.12 | **+1.3%** | ✅ |
| 1,316 | dtsort | 9373.21 | 9412.14 | -0.4% | ✅ |
| 1,478 | dtsort | 10673.18 | 10654.17 | +0.2% | ✅ |
| 1,660 | dtsort | 12066.78 | 12066.00 | +0.0% | ✅ |
| 1,865 | dtsort | 13947.36 | 13892.50 | +0.4% | ✅ |
| 2,095 | dtsort | 15594.85 | 15319.35 | **+1.8%** | ✅ |
| 2,354 | dtsort | 18569.31 | 18435.70 | **+0.7%** | ✅ |
| 2,644 | dtsort | 21269.48 | 21038.95 | **+1.1%** | ✅ |
| 2,970 | dtsort | 24057.04 | 23843.45 | **+0.9%** | ✅ |
| 3,336 | dtsort | 27424.11 | 27343.86 | +0.3% | ✅ |
| 3,748 | dtsort | 33020.16 | 32845.60 | **+0.5%** | ✅ |
| 4,210 | dtsort | 36953.00 | 36603.50 | **+1.0%** | ✅ |
| 4,729 | dtsort | 39895.77 | 39584.06 | **+0.8%** | ✅ |
| 5,312 | dtsort | 46600.46 | 46427.15 | +0.4% | ✅ |
| 5,967 | dtsort | 51315.68 | 50691.17 | **+1.2%** | ✅ |
| 6,703 | dtsort | 59172.64 | 58484.39 | **+1.2%** | ✅ |
| 7,530 | dtsort | 71794.55 | 71580.31 | +0.3% | ✅ |
| 8,458 | dtsort | 80099.08 | 79671.85 | **+0.5%** | ✅ |
| 9,502 | dtsort | 90851.49 | 90245.42 | **+0.7%** | ✅ |
| 10,673 | dtsort | 100516.38 | 98929.61 | **+1.6%** | ✅ |
| 11,990 | dtsort | 119109.54 | 119038.60 | +0.1% | ✅ |
| 13,468 | dtsort | 132702.71 | 131741.65 | **+0.7%** | ✅ |
| 15,129 | dtsort | 150277.71 | 150955.95 | -0.4% | ✅ |
| 16,995 | dtsort | 175196.98 | 175134.13 | +0.0% | ✅ |
| 19,091 | dtsort | 199905.23 | 197727.87 | **+1.1%** | ✅ |
| 21,445 | dtsort | 242607.95 | 242294.12 | +0.1% | ✅ |
| 24,090 | dtsort | 273670.30 | 269819.59 | **+1.4%** | ✅ |
| 27,061 | dtsort | 314896.31 | 313487.25 | +0.4% | ✅ |
| 30,398 | dtsort | 360737.90 | 355886.97 | **+1.4%** | ✅ |
| 34,147 | dtsort | 404555.82 | 403652.67 | +0.2% | ✅ |
| 38,358 | dtsort | 453781.36 | 452653.05 | +0.2% | ✅ |
| 43,089 | dtsort | 500772.24 | 495172.59 | **+1.1%** | ✅ |
| 48,403 | dtsort | 572970.10 | 564028.52 | **+1.6%** | ✅ |
| 54,372 | dtsort | 652647.71 | 643931.90 | **+1.4%** | ✅ |
| 61,077 | dtsort | 739558.73 | 726158.66 | **+1.8%** | ✅ |
| 68,609 | dtsort | 831479.93 | 816189.79 | **+1.9%** | ✅ |
| 77,071 | dtsort | 942582.94 | 921557.46 | **+2.3%** | ✅ |
| 86,575 | dtsort | 1095511.12 | 1066558.47 | **+2.7%** | ✅ |
| 97,252 | dtsort | 1271083.33 | 1247230.10 | **+1.9%** | ✅ |
| 109,246 | dtsort | 1436516.84 | 1410152.19 | **+1.9%** | ✅ |
| 122,718 | dtsort | 1665247.82 | 1631134.85 | **+2.1%** | ✅ |
| 137,852 | dtsort | 1887377.15 | 1829245.42 | **+3.2%** | ✅ |
| 154,853 | dtsort | 2086089.07 | 2018900.53 | **+3.3%** | ✅ |
| 173,950 | dtsort | 2435637.38 | 2367496.49 | **+2.9%** | ✅ |
| 195,402 | dtsort | 2757550.36 | 2692646.88 | **+2.4%** | ✅ |
| 219,500 | dtsort | 3139884.72 | 3073506.95 | **+2.2%** | ✅ |
| 246,569 | dtsort | 3566748.30 | 3481853.23 | **+2.4%** | ✅ |
| 276,977 | dtsort | 4027284.97 | 3910212.29 | **+3.0%** | ✅ |
| 311,135 | dtsort | 4512134.94 | 4399414.57 | **+2.6%** | ✅ |
| 349,506 | dtsort | 5273935.47 | 5150504.63 | **+2.4%** | ✅ |
| 392,608 | dtsort | 5920822.39 | 5779430.10 | **+2.4%** | ✅ |
| 441,026 | dtsort | 6707248.40 | 6552579.44 | **+2.4%** | ✅ |
| 495,415 | dtsort | 7611084.24 | 7382639.04 | **+3.1%** | ✅ |
| 556,512 | dtsort | 8676036.97 | 8508150.91 | **+2.0%** | ✅ |
| 625,143 | dtsort | 9562481.42 | 9342489.45 | **+2.4%** | ✅ |
| 702,238 | dtsort | 11032236.78 | 10836378.26 | **+1.8%** | ✅ |
| 788,841 | dtsort | 12265566.53 | 11947125.00 | **+2.7%** | ✅ |
| 886,124 | dtsort | 14059346.66 | 13782479.57 | **+2.0%** | ✅ |
| 995,405 | dtsort | 15899649.64 | 15523137.02 | **+2.4%** | ✅ |
| 1,118,162 | dtsort | 18200291.66 | 17837802.36 | **+2.0%** | ✅ |
| 1,256,058 | dtsort | 20541490.21 | 20134216.65 | **+2.0%** | ✅ |
| 1,410,960 | dtsort | 23397469.44 | 22923482.55 | **+2.1%** | ✅ |
| 1,584,966 | dtsort | 26403672.82 | 25829080.26 | **+2.2%** | ✅ |
| 1,780,430 | dtsort | 29910180.58 | 29200503.46 | **+2.4%** | ✅ |
| 2,000,000 | dtsort | 34510864.60 | 33666682.56 | **+2.5%** | ✅ |

#### GCC16 (IntNaked (Sorted))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 15.35 | 3.16 | **+385.7%** | ✅ |
| 3 | dtsort | 16.66 | 3.64 | **+358.2%** | ✅ |
| 4 | dtsort | 18.24 | 4.60 | **+296.3%** | ✅ |
| 5 | dtsort | 21.49 | 5.88 | **+265.5%** | ✅ |
| 6 | dtsort | 23.89 | 6.72 | **+255.4%** | ✅ |
| 7 | dtsort | 24.20 | 8.59 | **+181.8%** | ✅ |
| 8 | dtsort | 26.09 | 9.20 | **+183.5%** | ✅ |
| 9 | dtsort | 26.31 | 15.98 | **+64.7%** | ✅ |
| 10 | dtsort | 28.11 | 10.60 | **+165.2%** | ✅ |
| 20 | dtsort | 50.17 | 51.37 | -2.3% | ⚠️ |
| 22 | dtsort | 54.84 | 56.56 | -3.0% | ⚠️ |
| 25 | dtsort | 74.64 | 77.18 | -3.3% | ⚠️ |
| 28 | dtsort | 71.99 | 73.96 | -2.7% | ⚠️ |
| 32 | dtsort | 86.92 | 90.06 | -3.5% | ⚠️ |
| 36 | dtsort | 97.95 | 99.78 | -1.8% | ⚠️ |
| 40 | dtsort | 105.80 | 107.94 | -2.0% | ⚠️ |
| 45 | dtsort | 112.39 | 119.49 | -5.9% | ⚠️ |
| 51 | dtsort | 143.17 | 142.42 | **+0.5%** | ✅ |
| 57 | dtsort | 173.28 | 181.86 | -4.7% | ⚠️ |
| 64 | dtsort | 244.43 | 253.88 | -3.7% | ⚠️ |
| 72 | dtsort | 273.19 | 287.51 | -5.0% | ⚠️ |
| 81 | dtsort | 282.78 | 294.19 | -3.9% | ⚠️ |
| 91 | dtsort | 326.71 | 351.82 | -7.1% | ⚠️ |
| 102 | dtsort | 386.01 | 393.88 | -2.0% | ⚠️ |
| 114 | dtsort | 564.85 | 577.51 | -2.2% | ⚠️ |
| 129 | dtsort | 557.92 | 576.01 | -3.1% | ⚠️ |
| 144 | dtsort | 652.87 | 656.87 | -0.6% | ✅ |
| 162 | dtsort | 672.55 | 708.63 | -5.1% | ⚠️ |
| 182 | dtsort | 735.59 | 771.37 | -4.6% | ⚠️ |
| 205 | dtsort | 802.61 | 831.35 | -3.5% | ⚠️ |
| 230 | dtsort | 1313.17 | 1353.59 | -3.0% | ⚠️ |
| 258 | dtsort | 1370.62 | 1457.34 | -6.0% | ⚠️ |
| 290 | dtsort | 1529.93 | 1621.62 | -5.7% | ⚠️ |
| 326 | dtsort | 1613.63 | 1725.63 | -6.5% | ⚠️ |
| 366 | dtsort | 1855.76 | 2006.26 | -7.5% | ⚠️ |
| 411 | dtsort | 1943.28 | 2030.98 | -4.3% | ⚠️ |
| 462 | dtsort | 2858.63 | 3001.16 | -4.7% | ⚠️ |
| 519 | dtsort | 2931.30 | 3030.72 | -3.3% | ⚠️ |
| 583 | dtsort | 3349.46 | 3444.26 | -2.8% | ⚠️ |
| 655 | dtsort | 3516.19 | 3675.66 | -4.3% | ⚠️ |
| 736 | dtsort | 4254.25 | 4528.03 | -6.0% | ⚠️ |
| 826 | dtsort | 4694.28 | 4968.58 | -5.5% | ⚠️ |
| 928 | dtsort | 6513.98 | 6770.90 | -3.8% | ⚠️ |
| 1,043 | dtsort | 6793.00 | 6983.35 | -2.7% | ⚠️ |
| 1,171 | dtsort | 7403.19 | 7596.55 | -2.5% | ⚠️ |
| 1,316 | dtsort | 8597.65 | 8936.62 | -3.8% | ⚠️ |
| 1,478 | dtsort | 10123.94 | 10388.50 | -2.5% | ⚠️ |
| 1,660 | dtsort | 11174.33 | 11498.98 | -2.8% | ⚠️ |
| 1,865 | dtsort | 13810.00 | 14335.87 | -3.7% | ⚠️ |
| 2,095 | dtsort | 15023.68 | 15640.53 | -3.9% | ⚠️ |
| 2,354 | dtsort | 18429.96 | 18957.44 | -2.8% | ⚠️ |
| 2,644 | dtsort | 19930.59 | 20605.77 | -3.3% | ⚠️ |
| 2,970 | dtsort | 23050.91 | 23710.80 | -2.8% | ⚠️ |
| 3,336 | dtsort | 25257.44 | 25888.40 | -2.4% | ⚠️ |
| 3,748 | dtsort | 33312.54 | 34258.05 | -2.8% | ⚠️ |
| 4,210 | dtsort | 36350.83 | 37212.57 | -2.3% | ⚠️ |
| 4,729 | dtsort | 37353.70 | 38127.65 | -2.0% | ⚠️ |
| 5,312 | dtsort | 44090.51 | 44886.46 | -1.8% | ⚠️ |
| 5,967 | dtsort | 48519.08 | 49329.84 | -1.6% | ⚠️ |
| 6,703 | dtsort | 52241.14 | 53028.60 | -1.5% | ⚠️ |
| 7,530 | dtsort | 70070.61 | 71728.38 | -2.3% | ⚠️ |
| 8,458 | dtsort | 76731.76 | 77846.33 | -1.4% | ⚠️ |
| 9,502 | dtsort | 86681.18 | 88026.14 | -1.5% | ⚠️ |
| 10,673 | dtsort | 88374.53 | 89464.01 | -1.2% | ⚠️ |
| 11,990 | dtsort | 107890.51 | 108929.82 | -1.0% | ✅ |
| 13,468 | dtsort | 122344.54 | 123542.21 | -1.0% | ✅ |
| 15,129 | dtsort | 145641.50 | 143879.12 | **+1.2%** | ✅ |
| 16,995 | dtsort | 162978.52 | 162896.42 | +0.1% | ✅ |
| 19,091 | dtsort | 193338.79 | 192915.27 | +0.2% | ✅ |
| 21,445 | dtsort | 217279.15 | 217092.05 | +0.1% | ✅ |
| 24,090 | dtsort | 266223.89 | 266173.56 | +0.0% | ✅ |
| 27,061 | dtsort | 278972.18 | 280104.66 | -0.4% | ✅ |
| 30,398 | dtsort | 354244.38 | 352453.83 | **+0.5%** | ✅ |
| 34,147 | dtsort | 369570.62 | 368374.72 | +0.3% | ✅ |
| 38,358 | dtsort | 433914.86 | 433653.85 | +0.1% | ✅ |
| 43,089 | dtsort | 454540.53 | 450462.39 | **+0.9%** | ✅ |
| 48,403 | dtsort | 513853.91 | 511317.31 | +0.5% | ✅ |
| 54,372 | dtsort | 604832.72 | 602041.81 | +0.5% | ✅ |
| 61,077 | dtsort | 721392.84 | 712952.29 | **+1.2%** | ✅ |
| 68,609 | dtsort | 794631.17 | 781259.68 | **+1.7%** | ✅ |
| 77,071 | dtsort | 887023.21 | 872211.98 | **+1.7%** | ✅ |
| 86,575 | dtsort | 996189.11 | 977227.00 | **+1.9%** | ✅ |
| 97,252 | dtsort | 1170457.22 | 1157994.78 | **+1.1%** | ✅ |
| 109,246 | dtsort | 1306860.72 | 1292919.20 | **+1.1%** | ✅ |
| 122,718 | dtsort | 1563235.74 | 1555480.00 | +0.5% | ✅ |
| 137,852 | dtsort | 1729334.56 | 1710747.75 | **+1.1%** | ✅ |
| 154,853 | dtsort | 1835819.88 | 1814438.69 | **+1.2%** | ✅ |
| 173,950 | dtsort | 2209366.27 | 2202633.44 | +0.3% | ✅ |
| 195,402 | dtsort | 2444259.94 | 2440326.49 | +0.2% | ✅ |
| 219,500 | dtsort | 2726345.54 | 2721026.26 | +0.2% | ✅ |
| 246,569 | dtsort | 3235036.70 | 3209739.68 | **+0.8%** | ✅ |
| 276,977 | dtsort | 3557359.42 | 3531332.08 | **+0.7%** | ✅ |
| 311,135 | dtsort | 3974736.58 | 3955405.90 | +0.5% | ✅ |
| 349,506 | dtsort | 4747080.22 | 4726425.39 | +0.4% | ✅ |
| 392,608 | dtsort | 5277192.67 | 5252586.78 | +0.5% | ✅ |
| 441,026 | dtsort | 5904651.13 | 5886392.51 | +0.3% | ✅ |
| 495,415 | dtsort | 6736114.08 | 6706302.49 | +0.4% | ✅ |
| 556,512 | dtsort | 7831444.29 | 7772049.53 | **+0.8%** | ✅ |
| 625,143 | dtsort | 8339195.44 | 8298451.38 | +0.5% | ✅ |
| 702,238 | dtsort | 9958548.13 | 9937061.91 | +0.2% | ✅ |
| 788,841 | dtsort | 10603314.39 | 10337610.29 | **+2.6%** | ✅ |
| 886,124 | dtsort | 12314018.28 | 12272752.20 | +0.3% | ✅ |
| 995,405 | dtsort | 14613777.77 | 14532825.52 | **+0.6%** | ✅ |
| 1,118,162 | dtsort | 16879106.71 | 16787494.04 | **+0.5%** | ✅ |
| 1,256,058 | dtsort | 18929941.43 | 18854731.98 | +0.4% | ✅ |
| 1,410,960 | dtsort | 21474019.53 | 21343152.79 | **+0.6%** | ✅ |
| 1,584,966 | dtsort | 23876377.90 | 23728966.67 | **+0.6%** | ✅ |
| 1,780,430 | dtsort | 26678137.81 | 26576314.11 | +0.4% | ✅ |
| 2,000,000 | dtsort | 31591935.63 | 31370117.41 | **+0.7%** | ✅ |

#### GCC16 (WideRow)
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 220.47 | 190.78 | **+15.6%** | ✅ |
| 3 | dtsort | 366.03 | 266.87 | **+37.2%** | ✅ |
| 4 | dtsort | 612.71 | 349.65 | **+75.2%** | ✅ |
| 5 | dtsort | 724.46 | 473.24 | **+53.1%** | ✅ |
| 6 | dtsort | 952.78 | 806.95 | **+18.1%** | ✅ |
| 7 | dtsort | 1191.88 | 782.26 | **+52.4%** | ✅ |
| 8 | dtsort | 1407.31 | 885.16 | **+59.0%** | ✅ |
| 9 | dtsort | 1627.20 | 1002.52 | **+62.3%** | ✅ |
| 10 | dtsort | 2042.56 | 1225.86 | **+66.6%** | ✅ |
| 20 | dtsort | 7454.06 | 7043.13 | **+5.8%** | ✅ |
| 22 | dtsort | 8577.61 | 7616.76 | **+12.6%** | ✅ |
| 25 | dtsort | 9662.90 | 8586.33 | **+12.5%** | ✅ |
| 28 | dtsort | 12597.18 | 11271.99 | **+11.8%** | ✅ |
| 32 | dtsort | 14578.80 | 13036.56 | **+11.8%** | ✅ |
| 36 | dtsort | 16961.59 | 15386.83 | **+10.2%** | ✅ |
| 40 | dtsort | 18836.58 | 17066.03 | **+10.4%** | ✅ |
| 45 | dtsort | 21258.41 | 18973.96 | **+12.0%** | ✅ |
| 51 | dtsort | 24488.69 | 22177.05 | **+10.4%** | ✅ |
| 57 | dtsort | 32816.05 | 29810.70 | **+10.1%** | ✅ |
| 64 | dtsort | 40610.61 | 37594.36 | **+8.0%** | ✅ |
| 72 | dtsort | 46256.36 | 42815.71 | **+8.0%** | ✅ |
| 81 | dtsort | 53460.21 | 49336.84 | **+8.4%** | ✅ |
| 91 | dtsort | 59119.81 | 54627.07 | **+8.2%** | ✅ |
| 102 | dtsort | 65572.11 | 61090.24 | **+7.3%** | ✅ |
| 114 | dtsort | 72757.92 | 67573.96 | **+7.7%** | ✅ |
| 129 | dtsort | 84589.72 | 78054.18 | **+8.4%** | ✅ |
| 144 | dtsort | 93247.06 | 86150.04 | **+8.2%** | ✅ |
| 162 | dtsort | 104016.58 | 96368.48 | **+7.9%** | ✅ |
| 182 | dtsort | 118966.03 | 108766.94 | **+9.4%** | ✅ |
| 205 | dtsort | 135433.34 | 124637.25 | **+8.7%** | ✅ |
| 230 | dtsort | 183871.91 | 173390.17 | **+6.0%** | ✅ |
| 258 | dtsort | 205319.62 | 193633.14 | **+6.0%** | ✅ |
| 290 | dtsort | 230956.84 | 217196.15 | **+6.3%** | ✅ |
| 326 | dtsort | 262110.08 | 246471.63 | **+6.3%** | ✅ |
| 366 | dtsort | 291133.69 | 276055.97 | **+5.5%** | ✅ |
| 411 | dtsort | 336171.06 | 313644.95 | **+7.2%** | ✅ |
| 462 | dtsort | 369725.40 | 348467.71 | **+6.1%** | ✅ |
| 519 | dtsort | 422751.46 | 398079.56 | **+6.2%** | ✅ |
| 583 | dtsort | 474453.25 | 446849.00 | **+6.2%** | ✅ |
| 655 | dtsort | 534799.29 | 500098.69 | **+6.9%** | ✅ |
| 736 | dtsort | 589361.29 | 552794.63 | **+6.6%** | ✅ |
| 826 | dtsort | 662060.69 | 623117.73 | **+6.2%** | ✅ |
| 928 | dtsort | 886460.19 | 842777.68 | **+5.2%** | ✅ |
| 1,043 | dtsort | 1020442.46 | 967567.48 | **+5.5%** | ✅ |
| 1,171 | dtsort | 1138223.35 | 1080272.22 | **+5.4%** | ✅ |
| 1,316 | dtsort | 1261531.64 | 1201586.87 | **+5.0%** | ✅ |
| 1,478 | dtsort | 1432616.74 | 1359096.77 | **+5.4%** | ✅ |
| 1,660 | dtsort | 1608642.31 | 1519005.91 | **+5.9%** | ✅ |
| 1,865 | dtsort | 1851894.17 | 1745006.91 | **+6.1%** | ✅ |
| 2,095 | dtsort | 2060216.54 | 1950819.01 | **+5.6%** | ✅ |
| 2,354 | dtsort | 2309212.20 | 2181514.85 | **+5.9%** | ✅ |
| 2,644 | dtsort | 2611591.51 | 2466633.75 | **+5.9%** | ✅ |
| 2,970 | dtsort | 2974803.97 | 2812491.97 | **+5.8%** | ✅ |
| 3,336 | dtsort | 3369126.02 | 3205169.15 | **+5.1%** | ✅ |
| 3,748 | dtsort | 4432473.29 | 4271585.11 | **+3.8%** | ✅ |
| 4,210 | dtsort | 5209614.42 | 5084710.92 | **+2.5%** | ✅ |
| 4,729 | dtsort | 6301969.45 | 6146280.96 | **+2.5%** | ✅ |
| 5,312 | dtsort | 7565548.61 | 7405356.83 | **+2.2%** | ✅ |
| 5,967 | dtsort | 9154978.89 | 8974074.25 | **+2.0%** | ✅ |
| 6,703 | dtsort | 10940888.01 | 10681528.41 | **+2.4%** | ✅ |
| 7,530 | dtsort | 12656159.96 | 12345135.96 | **+2.5%** | ✅ |
| 8,458 | dtsort | 14590686.62 | 14246518.71 | **+2.4%** | ✅ |
| 9,502 | dtsort | 16709871.05 | 16321860.46 | **+2.4%** | ✅ |
| 10,673 | dtsort | 19396156.25 | 18986028.13 | **+2.2%** | ✅ |
| 11,990 | dtsort | 21827805.97 | 21381686.88 | **+2.1%** | ✅ |
| 13,468 | dtsort | 24988227.67 | 24428965.78 | **+2.3%** | ✅ |
| 15,129 | dtsort | 32828472.24 | 32191138.23 | **+2.0%** | ✅ |
| 16,995 | dtsort | 37146394.73 | 36475008.78 | **+1.8%** | ✅ |
| 19,091 | dtsort | 42115492.64 | 41379799.00 | **+1.8%** | ✅ |
| 21,445 | dtsort | 47695875.00 | 47086033.33 | **+1.3%** | ✅ |
| 24,090 | dtsort | 53185118.61 | 52653769.23 | **+1.0%** | ✅ |
| 27,061 | dtsort | 60537121.59 | 60038527.76 | **+0.8%** | ✅ |
| 30,398 | dtsort | 67515758.40 | 66927125.00 | **+0.9%** | ✅ |
| 34,147 | dtsort | 76916847.22 | 76751347.22 | +0.2% | ✅ |
| 38,358 | dtsort | 85500489.63 | 85257572.88 | +0.3% | ✅ |
| 43,089 | dtsort | 97293797.58 | 97044023.72 | +0.3% | ✅ |
| 48,403 | dtsort | 108845382.00 | 108854270.81 | -0.0% | ✅ |
| 54,372 | dtsort | 120237312.47 | 120508138.83 | -0.2% | ✅ |
| 61,077 | dtsort | 155068191.61 | 155382760.51 | -0.2% | ✅ |
| 68,609 | dtsort | 173865426.97 | 174413468.75 | -0.3% | ✅ |
| 77,071 | dtsort | 195494176.99 | 196102573.01 | -0.3% | ✅ |
| 86,575 | dtsort | 219248361.34 | 221464416.63 | -1.0% | ⚠️ |
| 97,252 | dtsort | 243436805.67 | 244483999.99 | -0.4% | ✅ |
| 109,246 | dtsort | 273720777.67 | 274755527.65 | -0.4% | ✅ |
| 122,718 | dtsort | 307562166.59 | 308654062.45 | -0.4% | ✅ |
| 137,852 | dtsort | 345256229.51 | 346466250.02 | -0.3% | ✅ |
| 154,853 | dtsort | 393299250.05 | 394639604.49 | -0.3% | ✅ |
| 173,950 | dtsort | 435725021.06 | 437367625.07 | -0.4% | ✅ |
| 195,402 | dtsort | 489381895.98 | 491335000.14 | -0.4% | ✅ |
| 219,500 | dtsort | 548939249.94 | 551108083.92 | -0.4% | ✅ |
| 246,569 | dtsort | 698721207.91 | 700295333.05 | -0.2% | ✅ |
| 276,977 | dtsort | 785032082.81 | 787169500.02 | -0.3% | ✅ |
| 311,135 | dtsort | 880314541.05 | 891516291.07 | -1.3% | ⚠️ |
| 349,506 | dtsort | 983342000.05 | 981615457.92 | +0.2% | ✅ |
| 392,608 | dtsort | 1096137457.99 | 1101204209.03 | -0.5% | ✅ |
| 441,026 | dtsort | 1235398833.87 | 1238196959.02 | -0.2% | ✅ |
| 495,415 | dtsort | 1401713042.05 | 1404738834.13 | -0.2% | ✅ |
| 556,512 | dtsort | 1556460208.96 | 1559960124.78 | -0.2% | ✅ |
| 625,143 | dtsort | 1766691708.01 | 1776407249.97 | -0.5% | ✅ |
| 702,238 | dtsort | 1963830875.00 | 1966973667.03 | -0.2% | ✅ |
| 788,841 | dtsort | 2228342000.16 | 2237498791.89 | -0.4% | ✅ |
| 886,124 | dtsort | 2472919208.00 | 2480027000.19 | -0.3% | ✅ |
| 995,405 | dtsort | 3106545374.96 | 3119352916.02 | -0.4% | ✅ |
| 1,118,162 | dtsort | 3493520458.00 | 3505504833.07 | -0.3% | ✅ |
| 1,256,058 | dtsort | 3921122333.03 | 3936025416.94 | -0.4% | ✅ |
| 1,410,960 | dtsort | 4398808167.08 | 4415974125.03 | -0.4% | ✅ |
| 1,584,966 | dtsort | 4947457958.94 | 4965701000.07 | -0.4% | ✅ |
| 1,780,430 | dtsort | 5560105249.98 | 5574060583.02 | -0.3% | ✅ |
| 2,000,000 | dtsort | 6243473625.04 | 6252550374.95 | -0.1% | ✅ |

#### GCC16 (WideRow (Reverse))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 221.27 | 190.20 | **+16.3%** | ✅ |
| 3 | dtsort | 366.96 | 266.83 | **+37.5%** | ✅ |
| 4 | dtsort | 598.25 | 347.70 | **+72.1%** | ✅ |
| 5 | dtsort | 841.32 | 390.60 | **+115.4%** | ✅ |
| 6 | dtsort | 1068.07 | 735.41 | **+45.2%** | ✅ |
| 7 | dtsort | 1327.50 | 844.34 | **+57.2%** | ✅ |
| 8 | dtsort | 1596.93 | 1108.86 | **+44.0%** | ✅ |
| 9 | dtsort | 1937.73 | 1222.62 | **+58.5%** | ✅ |
| 10 | dtsort | 2292.82 | 1449.05 | **+58.2%** | ✅ |
| 20 | dtsort | 8825.75 | 7704.45 | **+14.6%** | ✅ |
| 22 | dtsort | 10034.00 | 8824.57 | **+13.7%** | ✅ |
| 25 | dtsort | 11561.19 | 9772.12 | **+18.3%** | ✅ |
| 28 | dtsort | 14270.01 | 11841.87 | **+20.5%** | ✅ |
| 32 | dtsort | 15928.82 | 14001.17 | **+13.8%** | ✅ |
| 36 | dtsort | 18302.04 | 15846.53 | **+15.5%** | ✅ |
| 40 | dtsort | 21023.28 | 18300.25 | **+14.9%** | ✅ |
| 45 | dtsort | 24304.06 | 20924.53 | **+16.2%** | ✅ |
| 51 | dtsort | 27885.55 | 23888.78 | **+16.7%** | ✅ |
| 57 | dtsort | 35990.10 | 31898.13 | **+12.8%** | ✅ |
| 64 | dtsort | 44391.94 | 39569.31 | **+12.2%** | ✅ |
| 72 | dtsort | 49845.04 | 44841.79 | **+11.2%** | ✅ |
| 81 | dtsort | 58520.49 | 52377.17 | **+11.7%** | ✅ |
| 91 | dtsort | 64767.80 | 58032.71 | **+11.6%** | ✅ |
| 102 | dtsort | 71917.08 | 64702.84 | **+11.1%** | ✅ |
| 114 | dtsort | 80652.00 | 72479.78 | **+11.3%** | ✅ |
| 129 | dtsort | 92068.44 | 82657.14 | **+11.4%** | ✅ |
| 144 | dtsort | 99226.13 | 89798.20 | **+10.5%** | ✅ |
| 162 | dtsort | 113616.26 | 101763.91 | **+11.6%** | ✅ |
| 182 | dtsort | 127983.57 | 114619.85 | **+11.7%** | ✅ |
| 205 | dtsort | 146379.49 | 130783.23 | **+11.9%** | ✅ |
| 230 | dtsort | 196648.89 | 181353.11 | **+8.4%** | ✅ |
| 258 | dtsort | 220188.57 | 201729.27 | **+9.2%** | ✅ |
| 290 | dtsort | 248682.18 | 228051.99 | **+9.0%** | ✅ |
| 326 | dtsort | 277429.67 | 255001.10 | **+8.8%** | ✅ |
| 366 | dtsort | 312314.19 | 287633.79 | **+8.6%** | ✅ |
| 411 | dtsort | 354276.33 | 325270.95 | **+8.9%** | ✅ |
| 462 | dtsort | 396747.28 | 363808.96 | **+9.1%** | ✅ |
| 519 | dtsort | 449939.47 | 412523.65 | **+9.1%** | ✅ |
| 583 | dtsort | 501772.08 | 462193.82 | **+8.6%** | ✅ |
| 655 | dtsort | 566707.72 | 518445.14 | **+9.3%** | ✅ |
| 736 | dtsort | 618303.40 | 565272.14 | **+9.4%** | ✅ |
| 826 | dtsort | 706703.48 | 642771.97 | **+9.9%** | ✅ |
| 928 | dtsort | 923161.06 | 853802.20 | **+8.1%** | ✅ |
| 1,043 | dtsort | 1056303.05 | 983419.96 | **+7.4%** | ✅ |
| 1,171 | dtsort | 1184172.90 | 1099748.03 | **+7.7%** | ✅ |
| 1,316 | dtsort | 1306122.38 | 1223167.91 | **+6.8%** | ✅ |
| 1,478 | dtsort | 1500296.85 | 1388598.96 | **+8.0%** | ✅ |
| 1,660 | dtsort | 1697476.12 | 1528755.58 | **+11.0%** | ✅ |
| 1,865 | dtsort | 1913347.38 | 1785133.52 | **+7.2%** | ✅ |
| 2,095 | dtsort | 2135514.45 | 1991477.57 | **+7.2%** | ✅ |
| 2,354 | dtsort | 2394473.65 | 2230988.34 | **+7.3%** | ✅ |
| 2,644 | dtsort | 2672300.71 | 2494084.22 | **+7.1%** | ✅ |
| 2,970 | dtsort | 3073826.51 | 2853072.40 | **+7.7%** | ✅ |
| 3,336 | dtsort | 3415406.81 | 3226119.18 | **+5.9%** | ✅ |
| 3,748 | dtsort | 4536121.24 | 4235481.06 | **+7.1%** | ✅ |
| 4,210 | dtsort | 5372515.57 | 5124905.15 | **+4.8%** | ✅ |
| 4,729 | dtsort | 6799601.90 | 6469877.45 | **+5.1%** | ✅ |
| 5,312 | dtsort | 7831469.74 | 7526251.89 | **+4.1%** | ✅ |
| 5,967 | dtsort | 9705026.98 | 9386542.23 | **+3.4%** | ✅ |
| 6,703 | dtsort | 11512331.95 | 11126155.24 | **+3.5%** | ✅ |
| 7,530 | dtsort | 13174811.69 | 12807819.44 | **+2.9%** | ✅ |
| 8,458 | dtsort | 15266741.67 | 14857046.98 | **+2.8%** | ✅ |
| 9,502 | dtsort | 17571269.23 | 17091634.14 | **+2.8%** | ✅ |
| 10,673 | dtsort | 20897482.33 | 20281694.85 | **+3.0%** | ✅ |
| 11,990 | dtsort | 23201330.53 | 22603021.51 | **+2.6%** | ✅ |
| 13,468 | dtsort | 26236139.43 | 25680776.22 | **+2.2%** | ✅ |
| 15,129 | dtsort | 35197808.35 | 34284243.74 | **+2.7%** | ✅ |
| 16,995 | dtsort | 39539655.06 | 38513564.83 | **+2.7%** | ✅ |
| 19,091 | dtsort | 44763664.06 | 43771809.88 | **+2.3%** | ✅ |
| 21,445 | dtsort | 51017357.15 | 50012571.42 | **+2.0%** | ✅ |
| 24,090 | dtsort | 56719215.26 | 56047468.00 | **+1.2%** | ✅ |
| 27,061 | dtsort | 64641132.53 | 63891041.64 | **+1.2%** | ✅ |
| 30,398 | dtsort | 71949424.99 | 71598779.11 | +0.5% | ✅ |
| 34,147 | dtsort | 81853921.34 | 81385027.77 | **+0.6%** | ✅ |
| 38,358 | dtsort | 91098005.10 | 90576703.11 | **+0.6%** | ✅ |
| 43,089 | dtsort | 103968339.27 | 103332827.30 | **+0.6%** | ✅ |
| 48,403 | dtsort | 116175277.82 | 115674493.02 | +0.4% | ✅ |
| 54,372 | dtsort | 128024216.60 | 127706725.01 | +0.2% | ✅ |
| 61,077 | dtsort | 165480843.75 | 165055677.00 | +0.3% | ✅ |
| 68,609 | dtsort | 184418541.50 | 184166385.50 | +0.1% | ✅ |
| 77,071 | dtsort | 207035374.98 | 207037485.98 | -0.0% | ✅ |
| 86,575 | dtsort | 232997513.69 | 232422333.29 | +0.2% | ✅ |
| 97,252 | dtsort | 257804403.00 | 257848000.00 | -0.0% | ✅ |
| 109,246 | dtsort | 292683082.98 | 292700583.00 | -0.0% | ✅ |
| 122,718 | dtsort | 330087853.94 | 329890604.48 | +0.1% | ✅ |
| 137,852 | dtsort | 366963187.58 | 367402333.07 | -0.1% | ✅ |
| 154,853 | dtsort | 421460687.53 | 422745312.45 | -0.3% | ✅ |
| 173,950 | dtsort | 466996249.98 | 466610291.51 | +0.1% | ✅ |
| 195,402 | dtsort | 526083125.03 | 525746417.00 | +0.1% | ✅ |
| 219,500 | dtsort | 585538208.02 | 585510791.05 | +0.0% | ✅ |
| 246,569 | dtsort | 744634791.98 | 743855833.08 | +0.1% | ✅ |
| 276,977 | dtsort | 835461416.98 | 835040207.95 | +0.1% | ✅ |
| 311,135 | dtsort | 932089875.22 | 932103000.12 | -0.0% | ✅ |
| 349,506 | dtsort | 1044867791.00 | 1045231875.03 | -0.0% | ✅ |
| 392,608 | dtsort | 1163347875.00 | 1161560249.98 | +0.2% | ✅ |
| 441,026 | dtsort | 1316187958.00 | 1316534125.01 | -0.0% | ✅ |
| 495,415 | dtsort | 1484816584.04 | 1485267207.96 | -0.0% | ✅ |
| 556,512 | dtsort | 1648008499.99 | 1648164959.16 | -0.0% | ✅ |
| 625,143 | dtsort | 1878750999.92 | 1869608625.07 | +0.5% | ✅ |
| 702,238 | dtsort | 2103339874.88 | 2103481500.18 | -0.0% | ✅ |
| 788,841 | dtsort | 2386606000.13 | 2385070624.99 | +0.1% | ✅ |
| 886,124 | dtsort | 2623730083.00 | 2621533832.98 | +0.1% | ✅ |
| 995,405 | dtsort | 3313947916.96 | 3299629082.90 | +0.4% | ✅ |
| 1,118,162 | dtsort | 3714899542.04 | 3713829832.85 | +0.0% | ✅ |
| 1,256,058 | dtsort | 4162922957.91 | 4158238708.97 | +0.1% | ✅ |
| 1,410,960 | dtsort | 4639048916.99 | 4631710750.05 | +0.2% | ✅ |
| 1,584,966 | dtsort | 5273205708.01 | 5273412666.05 | -0.0% | ✅ |
| 1,780,430 | dtsort | 5905662458.16 | 5907574041.05 | -0.0% | ✅ |
| 2,000,000 | dtsort | 6586316667.03 | 6592226125.07 | -0.1% | ✅ |

#### GCC16 (WideRow (Sorted))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 182.22 | 84.51 | **+115.6%** | ✅ |
| 3 | dtsort | 292.34 | 122.42 | **+138.8%** | ✅ |
| 4 | dtsort | 489.60 | 171.67 | **+185.2%** | ✅ |
| 5 | dtsort | 611.70 | 225.30 | **+171.5%** | ✅ |
| 6 | dtsort | 790.55 | 282.75 | **+179.6%** | ✅ |
| 7 | dtsort | 916.45 | 324.45 | **+182.5%** | ✅ |
| 8 | dtsort | 1119.19 | 378.22 | **+195.9%** | ✅ |
| 9 | dtsort | 1265.61 | 417.48 | **+203.2%** | ✅ |
| 10 | dtsort | 1545.51 | 460.96 | **+235.3%** | ✅ |
| 20 | dtsort | 6587.81 | 5127.82 | **+28.5%** | ✅ |
| 22 | dtsort | 7234.24 | 5813.87 | **+24.4%** | ✅ |
| 25 | dtsort | 8160.99 | 6405.83 | **+27.4%** | ✅ |
| 28 | dtsort | 9852.63 | 7776.83 | **+26.7%** | ✅ |
| 32 | dtsort | 11703.45 | 9413.42 | **+24.3%** | ✅ |
| 36 | dtsort | 13759.13 | 10840.28 | **+26.9%** | ✅ |
| 40 | dtsort | 15816.99 | 12392.36 | **+27.6%** | ✅ |
| 45 | dtsort | 18073.78 | 14139.81 | **+27.8%** | ✅ |
| 51 | dtsort | 20243.23 | 15660.03 | **+29.3%** | ✅ |
| 57 | dtsort | 28098.14 | 23136.74 | **+21.4%** | ✅ |
| 64 | dtsort | 35800.88 | 29873.04 | **+19.8%** | ✅ |
| 72 | dtsort | 39757.31 | 33675.12 | **+18.1%** | ✅ |
| 81 | dtsort | 46487.74 | 39210.56 | **+18.6%** | ✅ |
| 91 | dtsort | 50280.71 | 42320.49 | **+18.8%** | ✅ |
| 102 | dtsort | 57352.31 | 48288.96 | **+18.8%** | ✅ |
| 114 | dtsort | 63755.22 | 53761.21 | **+18.6%** | ✅ |
| 129 | dtsort | 72769.61 | 61819.59 | **+17.7%** | ✅ |
| 144 | dtsort | 78688.16 | 66082.83 | **+19.1%** | ✅ |
| 162 | dtsort | 90408.99 | 75567.12 | **+19.6%** | ✅ |
| 182 | dtsort | 100811.46 | 85135.30 | **+18.4%** | ✅ |
| 205 | dtsort | 115331.26 | 98337.95 | **+17.3%** | ✅ |
| 230 | dtsort | 163266.86 | 142714.26 | **+14.4%** | ✅ |
| 258 | dtsort | 182290.65 | 160312.83 | **+13.7%** | ✅ |
| 290 | dtsort | 204608.96 | 179941.77 | **+13.7%** | ✅ |
| 326 | dtsort | 231515.35 | 202583.65 | **+14.3%** | ✅ |
| 366 | dtsort | 259228.42 | 227607.37 | **+13.9%** | ✅ |
| 411 | dtsort | 286997.67 | 251931.26 | **+13.9%** | ✅ |
| 462 | dtsort | 329038.19 | 286329.81 | **+14.9%** | ✅ |
| 519 | dtsort | 366263.05 | 320405.69 | **+14.3%** | ✅ |
| 583 | dtsort | 411297.70 | 360164.40 | **+14.2%** | ✅ |
| 655 | dtsort | 459966.10 | 402208.95 | **+14.4%** | ✅ |
| 736 | dtsort | 508673.00 | 444840.31 | **+14.3%** | ✅ |
| 826 | dtsort | 578934.05 | 507575.25 | **+14.1%** | ✅ |
| 928 | dtsort | 783962.33 | 702988.80 | **+11.5%** | ✅ |
| 1,043 | dtsort | 896123.89 | 801368.00 | **+11.8%** | ✅ |
| 1,171 | dtsort | 1000047.53 | 902596.96 | **+10.8%** | ✅ |
| 1,316 | dtsort | 1112340.49 | 1003209.89 | **+10.9%** | ✅ |
| 1,478 | dtsort | 1279436.78 | 1160913.96 | **+10.2%** | ✅ |
| 1,660 | dtsort | 1415156.71 | 1279841.08 | **+10.6%** | ✅ |
| 1,865 | dtsort | 1620935.75 | 1515419.97 | **+7.0%** | ✅ |
| 2,095 | dtsort | 1815291.67 | 1653278.53 | **+9.8%** | ✅ |
| 2,354 | dtsort | 2055531.63 | 1845404.75 | **+11.4%** | ✅ |
| 2,644 | dtsort | 2272994.88 | 2070997.18 | **+9.8%** | ✅ |
| 2,970 | dtsort | 2629629.75 | 2403829.59 | **+9.4%** | ✅ |
| 3,336 | dtsort | 2928648.45 | 2670677.45 | **+9.7%** | ✅ |
| 3,748 | dtsort | 3948521.20 | 3775975.96 | **+4.6%** | ✅ |
| 4,210 | dtsort | 4880633.53 | 4729391.02 | **+3.2%** | ✅ |
| 4,729 | dtsort | 5952671.12 | 5592026.44 | **+6.4%** | ✅ |
| 5,312 | dtsort | 6973769.12 | 6794967.59 | **+2.6%** | ✅ |
| 5,967 | dtsort | 8708861.84 | 8090112.87 | **+7.6%** | ✅ |
| 6,703 | dtsort | 10234618.78 | 9705094.98 | **+5.5%** | ✅ |
| 7,530 | dtsort | 11674431.76 | 11408204.81 | **+2.3%** | ✅ |
| 8,458 | dtsort | 13670105.00 | 13146088.94 | **+4.0%** | ✅ |
| 9,502 | dtsort | 15707726.32 | 15057989.13 | **+4.3%** | ✅ |
| 10,673 | dtsort | 18529686.42 | 17618022.44 | **+5.2%** | ✅ |
| 11,990 | dtsort | 20805682.62 | 19743319.06 | **+5.4%** | ✅ |
| 13,468 | dtsort | 23538548.64 | 22214221.77 | **+6.0%** | ✅ |
| 15,129 | dtsort | 31974047.36 | 30259422.09 | **+5.7%** | ✅ |
| 16,995 | dtsort | 35413468.75 | 33725444.43 | **+5.0%** | ✅ |
| 19,091 | dtsort | 40423517.17 | 38236215.28 | **+5.7%** | ✅ |
| 21,445 | dtsort | 46040569.46 | 43693815.13 | **+5.4%** | ✅ |
| 24,090 | dtsort | 51610770.87 | 48885473.22 | **+5.6%** | ✅ |
| 27,061 | dtsort | 58027604.17 | 55051013.92 | **+5.4%** | ✅ |
| 30,398 | dtsort | 65134325.81 | 61519037.91 | **+5.9%** | ✅ |
| 34,147 | dtsort | 72736037.50 | 69277170.81 | **+5.0%** | ✅ |
| 38,358 | dtsort | 82721527.77 | 78572726.89 | **+5.3%** | ✅ |
| 43,089 | dtsort | 92755958.38 | 88790713.61 | **+4.5%** | ✅ |
| 48,403 | dtsort | 102811559.57 | 98417916.70 | **+4.5%** | ✅ |
| 54,372 | dtsort | 115365152.83 | 110200222.15 | **+4.7%** | ✅ |
| 61,077 | dtsort | 149678766.59 | 144380733.37 | **+3.7%** | ✅ |
| 68,609 | dtsort | 169191145.98 | 160320176.98 | **+5.5%** | ✅ |
| 77,071 | dtsort | 187263843.72 | 180594416.74 | **+3.7%** | ✅ |
| 86,575 | dtsort | 209783750.04 | 202897041.65 | **+3.4%** | ✅ |
| 97,252 | dtsort | 234186708.31 | 226473250.04 | **+3.4%** | ✅ |
| 109,246 | dtsort | 266534458.30 | 256956055.68 | **+3.7%** | ✅ |
| 122,718 | dtsort | 301131082.93 | 290613520.54 | **+3.6%** | ✅ |
| 137,852 | dtsort | 333064916.43 | 320650666.95 | **+3.9%** | ✅ |
| 154,853 | dtsort | 382671208.00 | 370650270.95 | **+3.2%** | ✅ |
| 173,950 | dtsort | 425481604.41 | 411381667.01 | **+3.4%** | ✅ |
| 195,402 | dtsort | 478797937.51 | 460234875.09 | **+4.0%** | ✅ |
| 219,500 | dtsort | 534733416.05 | 516008040.87 | **+3.6%** | ✅ |
| 246,569 | dtsort | 686418500.03 | 664668624.99 | **+3.3%** | ✅ |
| 276,977 | dtsort | 763193167.05 | 747447291.85 | **+2.1%** | ✅ |
| 311,135 | dtsort | 859144459.01 | 826905250.08 | **+3.9%** | ✅ |
| 349,506 | dtsort | 961337832.97 | 930522708.92 | **+3.3%** | ✅ |
| 392,608 | dtsort | 1071647292.00 | 1036111083.11 | **+3.4%** | ✅ |
| 441,026 | dtsort | 1216512167.13 | 1181193999.95 | **+3.0%** | ✅ |
| 495,415 | dtsort | 1368988208.01 | 1322280999.97 | **+3.5%** | ✅ |
| 556,512 | dtsort | 1514731333.13 | 1468557499.81 | **+3.1%** | ✅ |
| 625,143 | dtsort | 1712033499.96 | 1658644957.93 | **+3.2%** | ✅ |
| 702,238 | dtsort | 1939709041.97 | 1879907167.05 | **+3.2%** | ✅ |
| 788,841 | dtsort | 2185477708.00 | 2118159167.00 | **+3.2%** | ✅ |
| 886,124 | dtsort | 2419607124.99 | 2336795583.13 | **+3.5%** | ✅ |
| 995,405 | dtsort | 3050752332.90 | 2967811791.92 | **+2.8%** | ✅ |
| 1,118,162 | dtsort | 3443537291.14 | 3347459750.02 | **+2.9%** | ✅ |
| 1,256,058 | dtsort | 3870198833.99 | 3748462332.88 | **+3.2%** | ✅ |
| 1,410,960 | dtsort | 4303571624.91 | 4179617916.00 | **+3.0%** | ✅ |
| 1,584,966 | dtsort | 4911613250.15 | 4781852249.99 | **+2.7%** | ✅ |
| 1,780,430 | dtsort | 5504133458.02 | 5351004291.79 | **+2.9%** | ✅ |
| 2,000,000 | dtsort | 6119505208.92 | 5943337000.21 | **+3.0%** | ✅ |

#### LLVM19 (IntNaked)
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 12.54 | 2.47 | **+407.4%** | ✅ |
| 3 | dtsort | 18.02 | 3.03 | **+494.8%** | ✅ |
| 4 | dtsort | 18.47 | 3.74 | **+394.4%** | ✅ |
| 5 | dtsort | 21.64 | 4.20 | **+415.4%** | ✅ |
| 6 | dtsort | 23.65 | 8.20 | **+188.4%** | ✅ |
| 7 | dtsort | 21.91 | 10.81 | **+102.7%** | ✅ |
| 8 | dtsort | 23.69 | 12.27 | **+93.1%** | ✅ |
| 9 | dtsort | 25.83 | 15.19 | **+70.1%** | ✅ |
| 10 | dtsort | 27.29 | 12.34 | **+121.1%** | ✅ |
| 20 | dtsort | 82.51 | 63.11 | **+30.7%** | ✅ |
| 22 | dtsort | 93.37 | 70.65 | **+32.1%** | ✅ |
| 25 | dtsort | 103.79 | 84.48 | **+22.9%** | ✅ |
| 28 | dtsort | 113.29 | 100.22 | **+13.0%** | ✅ |
| 32 | dtsort | 134.48 | 114.83 | **+17.1%** | ✅ |
| 36 | dtsort | 156.38 | 137.89 | **+13.4%** | ✅ |
| 40 | dtsort | 182.95 | 153.83 | **+18.9%** | ✅ |
| 45 | dtsort | 206.71 | 227.85 | -9.3% | ⚠️ |
| 51 | dtsort | 240.91 | 268.00 | -10.1% | ⚠️ |
| 57 | dtsort | 274.21 | 303.40 | -9.6% | ⚠️ |
| 64 | dtsort | 316.01 | 349.98 | -9.7% | ⚠️ |
| 72 | dtsort | 466.89 | 399.59 | **+16.8%** | ✅ |
| 81 | dtsort | 554.38 | 458.97 | **+20.8%** | ✅ |
| 91 | dtsort | 639.69 | 542.54 | **+17.9%** | ✅ |
| 102 | dtsort | 727.43 | 648.14 | **+12.2%** | ✅ |
| 114 | dtsort | 832.52 | 1058.20 | -21.3% | ⚠️ |
| 129 | dtsort | 953.94 | 866.31 | **+10.1%** | ✅ |
| 144 | dtsort | 1050.20 | 972.62 | **+8.0%** | ✅ |
| 162 | dtsort | 1186.35 | 1108.70 | **+7.0%** | ✅ |
| 182 | dtsort | 1367.74 | 1453.62 | -5.9% | ⚠️ |
| 205 | dtsort | 1583.49 | 1675.64 | -5.5% | ⚠️ |
| 230 | dtsort | 1841.36 | 1922.05 | -4.2% | ⚠️ |
| 258 | dtsort | 2223.37 | 2274.68 | -2.3% | ⚠️ |
| 290 | dtsort | 2879.81 | 2561.26 | **+12.4%** | ✅ |
| 326 | dtsort | 3300.78 | 2941.55 | **+12.2%** | ✅ |
| 366 | dtsort | 3812.25 | 3452.93 | **+10.4%** | ✅ |
| 411 | dtsort | 4361.87 | 4013.92 | **+8.7%** | ✅ |
| 462 | dtsort | 4970.62 | 4621.43 | **+7.6%** | ✅ |
| 519 | dtsort | 5648.22 | 5343.25 | **+5.7%** | ✅ |
| 583 | dtsort | 6356.00 | 6113.94 | **+4.0%** | ✅ |
| 655 | dtsort | 7276.38 | 7054.71 | **+3.1%** | ✅ |
| 736 | dtsort | 8350.36 | 8648.93 | -3.5% | ⚠️ |
| 826 | dtsort | 9568.47 | 9893.00 | -3.3% | ⚠️ |
| 928 | dtsort | 11006.42 | 11338.08 | -2.9% | ⚠️ |
| 1,043 | dtsort | 13091.74 | 12923.64 | **+1.3%** | ✅ |
| 1,171 | dtsort | 15985.45 | 14560.67 | **+9.8%** | ✅ |
| 1,316 | dtsort | 18380.08 | 16759.59 | **+9.7%** | ✅ |
| 1,478 | dtsort | 21060.92 | 19444.92 | **+8.3%** | ✅ |
| 1,660 | dtsort | 24075.26 | 22417.38 | **+7.4%** | ✅ |
| 1,865 | dtsort | 27128.34 | 25707.54 | **+5.5%** | ✅ |
| 2,095 | dtsort | 30420.60 | 29469.60 | **+3.2%** | ✅ |
| 2,354 | dtsort | 34605.03 | 33628.52 | **+2.9%** | ✅ |
| 2,644 | dtsort | 39448.23 | 38624.62 | **+2.1%** | ✅ |
| 2,970 | dtsort | 45041.97 | 46183.24 | -2.5% | ⚠️ |
| 3,336 | dtsort | 51356.44 | 52481.50 | -2.1% | ⚠️ |
| 3,748 | dtsort | 58379.03 | 59594.20 | -2.0% | ⚠️ |
| 4,210 | dtsort | 69564.78 | 68207.24 | **+2.0%** | ✅ |
| 4,729 | dtsort | 82700.94 | 76923.23 | **+7.5%** | ✅ |
| 5,312 | dtsort | 94458.09 | 88084.24 | **+7.2%** | ✅ |
| 5,967 | dtsort | 108106.18 | 100944.14 | **+7.1%** | ✅ |
| 6,703 | dtsort | 122927.63 | 116170.63 | **+5.8%** | ✅ |
| 7,530 | dtsort | 138771.35 | 133552.83 | **+3.9%** | ✅ |
| 8,458 | dtsort | 157042.39 | 153330.15 | **+2.4%** | ✅ |
| 9,502 | dtsort | 178542.51 | 174288.37 | **+2.4%** | ✅ |
| 10,673 | dtsort | 202507.91 | 199430.83 | **+1.5%** | ✅ |
| 11,990 | dtsort | 229765.90 | 233299.18 | -1.5% | ⚠️ |
| 13,468 | dtsort | 260466.20 | 264441.03 | -1.5% | ⚠️ |
| 15,129 | dtsort | 298505.91 | 302133.37 | -1.2% | ⚠️ |
| 16,995 | dtsort | 352125.92 | 342536.69 | **+2.8%** | ✅ |
| 19,091 | dtsort | 412773.08 | 385853.81 | **+7.0%** | ✅ |
| 21,445 | dtsort | 470011.69 | 439734.66 | **+6.9%** | ✅ |
| 24,090 | dtsort | 537420.13 | 503492.42 | **+6.7%** | ✅ |
| 27,061 | dtsort | 608541.20 | 578642.26 | **+5.2%** | ✅ |
| 30,398 | dtsort | 690408.34 | 662636.27 | **+4.2%** | ✅ |
| 34,147 | dtsort | 784973.48 | 758165.48 | **+3.5%** | ✅ |
| 38,358 | dtsort | 893538.71 | 855528.19 | **+4.4%** | ✅ |
| 43,089 | dtsort | 1022348.44 | 992565.00 | **+3.0%** | ✅ |
| 48,403 | dtsort | 1182458.19 | 1182123.21 | +0.0% | ✅ |
| 54,372 | dtsort | 1360381.25 | 1340639.03 | **+1.5%** | ✅ |
| 61,077 | dtsort | 1561730.54 | 1523983.39 | **+2.5%** | ✅ |
| 68,609 | dtsort | 1826182.20 | 1767397.49 | **+3.3%** | ✅ |
| 77,071 | dtsort | 2139682.35 | 2007042.52 | **+6.6%** | ✅ |
| 86,575 | dtsort | 2435054.68 | 2259778.10 | **+7.8%** | ✅ |
| 97,252 | dtsort | 2800879.21 | 2622259.79 | **+6.8%** | ✅ |
| 109,246 | dtsort | 3164388.19 | 3023051.13 | **+4.7%** | ✅ |
| 122,718 | dtsort | 3554078.64 | 3421206.27 | **+3.9%** | ✅ |
| 137,852 | dtsort | 4056225.39 | 3957662.14 | **+2.5%** | ✅ |
| 154,853 | dtsort | 4598927.50 | 4502866.34 | **+2.1%** | ✅ |
| 173,950 | dtsort | 5238079.20 | 5157401.94 | **+1.6%** | ✅ |
| 195,402 | dtsort | 5956057.48 | 5997732.25 | -0.7% | ✅ |
| 219,500 | dtsort | 6819905.12 | 6856069.58 | -0.5% | ✅ |
| 246,569 | dtsort | 7705461.57 | 7682029.17 | +0.3% | ✅ |
| 276,977 | dtsort | 8929262.45 | 8796918.77 | **+1.5%** | ✅ |
| 311,135 | dtsort | 10341316.55 | 10035365.33 | **+3.0%** | ✅ |
| 349,506 | dtsort | 11737307.20 | 11217217.74 | **+4.6%** | ✅ |
| 392,608 | dtsort | 13330699.52 | 12702336.37 | **+4.9%** | ✅ |
| 441,026 | dtsort | 14989700.19 | 14436026.92 | **+3.8%** | ✅ |
| 495,415 | dtsort | 16972381.10 | 16505026.79 | **+2.8%** | ✅ |
| 556,512 | dtsort | 19129494.22 | 18596044.97 | **+2.9%** | ✅ |
| 625,143 | dtsort | 21602317.72 | 21047109.85 | **+2.6%** | ✅ |
| 702,238 | dtsort | 24504034.21 | 24179327.58 | **+1.3%** | ✅ |
| 788,841 | dtsort | 27585304.99 | 27628086.65 | -0.2% | ✅ |
| 886,124 | dtsort | 31332035.99 | 31352104.17 | -0.1% | ✅ |
| 995,405 | dtsort | 35590420.85 | 35448785.41 | +0.4% | ✅ |
| 1,118,162 | dtsort | 40713990.23 | 40006090.71 | **+1.8%** | ✅ |
| 1,256,058 | dtsort | 46882386.13 | 45442363.93 | **+3.2%** | ✅ |
| 1,410,960 | dtsort | 52997253.23 | 50506690.43 | **+4.9%** | ✅ |
| 1,584,966 | dtsort | 59957118.08 | 57129673.58 | **+4.9%** | ✅ |
| 1,780,430 | dtsort | 67849375.00 | 65199219.73 | **+4.1%** | ✅ |
| 2,000,000 | dtsort | 76416620.34 | 74177555.56 | **+3.0%** | ✅ |

#### LLVM19 (IntNaked (Reverse))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 12.44 | 2.45 | **+407.2%** | ✅ |
| 3 | dtsort | 17.81 | 3.03 | **+488.8%** | ✅ |
| 4 | dtsort | 18.54 | 3.96 | **+368.7%** | ✅ |
| 5 | dtsort | 21.42 | 4.06 | **+427.5%** | ✅ |
| 6 | dtsort | 24.03 | 11.73 | **+105.0%** | ✅ |
| 7 | dtsort | 23.00 | 9.14 | **+151.7%** | ✅ |
| 8 | dtsort | 24.99 | 11.01 | **+126.9%** | ✅ |
| 9 | dtsort | 27.22 | 11.89 | **+129.0%** | ✅ |
| 10 | dtsort | 27.78 | 11.18 | **+148.4%** | ✅ |
| 20 | dtsort | 67.62 | 52.43 | **+29.0%** | ✅ |
| 22 | dtsort | 76.41 | 56.33 | **+35.7%** | ✅ |
| 25 | dtsort | 86.24 | 69.42 | **+24.2%** | ✅ |
| 28 | dtsort | 92.71 | 78.55 | **+18.0%** | ✅ |
| 32 | dtsort | 98.10 | 84.74 | **+15.8%** | ✅ |
| 36 | dtsort | 125.40 | 104.88 | **+19.6%** | ✅ |
| 40 | dtsort | 137.80 | 107.75 | **+27.9%** | ✅ |
| 45 | dtsort | 162.84 | 177.45 | -8.2% | ⚠️ |
| 51 | dtsort | 191.84 | 198.08 | -3.1% | ⚠️ |
| 57 | dtsort | 221.39 | 220.68 | +0.3% | ✅ |
| 64 | dtsort | 248.73 | 237.51 | **+4.7%** | ✅ |
| 72 | dtsort | 341.08 | 273.92 | **+24.5%** | ✅ |
| 81 | dtsort | 381.29 | 311.73 | **+22.3%** | ✅ |
| 91 | dtsort | 449.29 | 367.32 | **+22.3%** | ✅ |
| 102 | dtsort | 510.03 | 432.30 | **+18.0%** | ✅ |
| 114 | dtsort | 554.58 | 692.55 | -19.9% | ⚠️ |
| 129 | dtsort | 611.44 | 533.31 | **+14.7%** | ✅ |
| 144 | dtsort | 684.93 | 590.04 | **+16.1%** | ✅ |
| 162 | dtsort | 792.09 | 695.96 | **+13.8%** | ✅ |
| 182 | dtsort | 916.99 | 965.25 | -5.0% | ⚠️ |
| 205 | dtsort | 1081.53 | 1099.17 | -1.6% | ⚠️ |
| 230 | dtsort | 1237.12 | 1218.03 | **+1.6%** | ✅ |
| 258 | dtsort | 1424.52 | 1388.55 | **+2.6%** | ✅ |
| 290 | dtsort | 1802.84 | 1528.88 | **+17.9%** | ✅ |
| 326 | dtsort | 2096.42 | 1780.49 | **+17.7%** | ✅ |
| 366 | dtsort | 2368.97 | 2059.46 | **+15.0%** | ✅ |
| 411 | dtsort | 2709.54 | 2398.50 | **+13.0%** | ✅ |
| 462 | dtsort | 3046.51 | 2774.43 | **+9.8%** | ✅ |
| 519 | dtsort | 3376.86 | 3104.19 | **+8.8%** | ✅ |
| 583 | dtsort | 3841.18 | 3460.27 | **+11.0%** | ✅ |
| 655 | dtsort | 4346.09 | 4054.11 | **+7.2%** | ✅ |
| 736 | dtsort | 5060.61 | 5191.99 | -2.5% | ⚠️ |
| 826 | dtsort | 5726.30 | 5861.27 | -2.3% | ⚠️ |
| 928 | dtsort | 6659.64 | 6565.71 | **+1.4%** | ✅ |
| 1,043 | dtsort | 7703.79 | 7426.11 | **+3.7%** | ✅ |
| 1,171 | dtsort | 9342.81 | 8220.08 | **+13.7%** | ✅ |
| 1,316 | dtsort | 10722.18 | 9501.61 | **+12.8%** | ✅ |
| 1,478 | dtsort | 12163.19 | 10927.19 | **+11.3%** | ✅ |
| 1,660 | dtsort | 13900.83 | 12580.06 | **+10.5%** | ✅ |
| 1,865 | dtsort | 15492.75 | 14485.05 | **+7.0%** | ✅ |
| 2,095 | dtsort | 17214.56 | 16190.64 | **+6.3%** | ✅ |
| 2,354 | dtsort | 19409.87 | 18138.44 | **+7.0%** | ✅ |
| 2,644 | dtsort | 22103.30 | 21173.53 | **+4.4%** | ✅ |
| 2,970 | dtsort | 25494.40 | 26155.41 | -2.5% | ⚠️ |
| 3,336 | dtsort | 28795.50 | 29290.79 | -1.7% | ⚠️ |
| 3,748 | dtsort | 32907.32 | 33090.77 | -0.6% | ✅ |
| 4,210 | dtsort | 38600.78 | 37058.05 | **+4.2%** | ✅ |
| 4,729 | dtsort | 46188.28 | 41366.62 | **+11.7%** | ✅ |
| 5,312 | dtsort | 52359.44 | 47511.00 | **+10.2%** | ✅ |
| 5,967 | dtsort | 59639.85 | 54424.20 | **+9.6%** | ✅ |
| 6,703 | dtsort | 68296.05 | 62634.04 | **+9.0%** | ✅ |
| 7,530 | dtsort | 75890.44 | 71776.38 | **+5.7%** | ✅ |
| 8,458 | dtsort | 84536.88 | 80259.34 | **+5.3%** | ✅ |
| 9,502 | dtsort | 94998.15 | 88536.32 | **+7.3%** | ✅ |
| 10,673 | dtsort | 107412.84 | 104228.36 | **+3.1%** | ✅ |
| 11,990 | dtsort | 122971.37 | 125012.19 | -1.6% | ⚠️ |
| 13,468 | dtsort | 138157.10 | 140413.25 | -1.6% | ⚠️ |
| 15,129 | dtsort | 157898.99 | 157443.72 | +0.3% | ✅ |
| 16,995 | dtsort | 185374.59 | 175101.63 | **+5.9%** | ✅ |
| 19,091 | dtsort | 220134.70 | 194222.80 | **+13.3%** | ✅ |
| 21,445 | dtsort | 249776.19 | 224727.05 | **+11.1%** | ✅ |
| 24,090 | dtsort | 280421.79 | 256655.76 | **+9.3%** | ✅ |
| 27,061 | dtsort | 314910.64 | 289736.60 | **+8.7%** | ✅ |
| 30,398 | dtsort | 351871.71 | 330898.79 | **+6.3%** | ✅ |
| 34,147 | dtsort | 394765.40 | 369284.34 | **+6.9%** | ✅ |
| 38,358 | dtsort | 439529.28 | 402192.74 | **+9.3%** | ✅ |
| 43,089 | dtsort | 502396.64 | 481833.13 | **+4.3%** | ✅ |
| 48,403 | dtsort | 571148.81 | 584371.62 | -2.3% | ⚠️ |
| 54,372 | dtsort | 651669.96 | 652460.53 | -0.1% | ✅ |
| 61,077 | dtsort | 754923.18 | 743547.92 | **+1.5%** | ✅ |
| 68,609 | dtsort | 895478.04 | 816136.41 | **+9.7%** | ✅ |
| 77,071 | dtsort | 1081718.26 | 911508.64 | **+18.7%** | ✅ |
| 86,575 | dtsort | 1216055.26 | 1060257.50 | **+14.7%** | ✅ |
| 97,252 | dtsort | 1396495.76 | 1246209.77 | **+12.1%** | ✅ |
| 109,246 | dtsort | 1545009.31 | 1398914.89 | **+10.4%** | ✅ |
| 122,718 | dtsort | 1716030.37 | 1584346.30 | **+8.3%** | ✅ |
| 137,852 | dtsort | 1939796.89 | 1772770.24 | **+9.4%** | ✅ |
| 154,853 | dtsort | 2168415.36 | 1956196.88 | **+10.8%** | ✅ |
| 173,950 | dtsort | 2487274.43 | 2392948.50 | **+3.9%** | ✅ |
| 195,402 | dtsort | 2798923.75 | 2822555.27 | -0.8% | ✅ |
| 219,500 | dtsort | 3236477.24 | 3142522.15 | **+3.0%** | ✅ |
| 246,569 | dtsort | 3628652.34 | 3508866.37 | **+3.4%** | ✅ |
| 276,977 | dtsort | 4337458.85 | 3945952.41 | **+9.9%** | ✅ |
| 311,135 | dtsort | 4998297.36 | 4341294.27 | **+15.1%** | ✅ |
| 349,506 | dtsort | 5719851.77 | 5043544.71 | **+13.4%** | ✅ |
| 392,608 | dtsort | 6318120.83 | 5686206.97 | **+11.1%** | ✅ |
| 441,026 | dtsort | 7145153.35 | 6464686.72 | **+10.5%** | ✅ |
| 495,415 | dtsort | 7906491.96 | 7251089.41 | **+9.0%** | ✅ |
| 556,512 | dtsort | 8814600.22 | 8038198.65 | **+9.7%** | ✅ |
| 625,143 | dtsort | 9971882.14 | 8940280.45 | **+11.5%** | ✅ |
| 702,238 | dtsort | 11391424.87 | 10987885.59 | **+3.7%** | ✅ |
| 788,841 | dtsort | 12694778.04 | 12520468.75 | **+1.4%** | ✅ |
| 886,124 | dtsort | 14672264.77 | 14066185.84 | **+4.3%** | ✅ |
| 995,405 | dtsort | 16467327.38 | 15821178.98 | **+4.1%** | ✅ |
| 1,118,162 | dtsort | 19002831.08 | 17318453.13 | **+9.7%** | ✅ |
| 1,256,058 | dtsort | 22105979.16 | 19622280.09 | **+12.7%** | ✅ |
| 1,410,960 | dtsort | 25018385.40 | 22270633.07 | **+12.3%** | ✅ |
| 1,584,966 | dtsort | 27533248.36 | 24975569.93 | **+10.2%** | ✅ |
| 1,780,430 | dtsort | 31330375.00 | 28450250.00 | **+10.1%** | ✅ |
| 2,000,000 | dtsort | 34758252.06 | 31920125.00 | **+8.9%** | ✅ |

#### LLVM19 (IntNaked (Sorted))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 12.64 | 2.66 | **+374.7%** | ✅ |
| 3 | dtsort | 17.25 | 3.13 | **+450.4%** | ✅ |
| 4 | dtsort | 18.24 | 4.09 | **+346.2%** | ✅ |
| 5 | dtsort | 20.55 | 4.57 | **+349.8%** | ✅ |
| 6 | dtsort | 21.79 | 6.18 | **+252.7%** | ✅ |
| 7 | dtsort | 21.82 | 9.27 | **+135.4%** | ✅ |
| 8 | dtsort | 23.05 | 9.19 | **+150.7%** | ✅ |
| 9 | dtsort | 23.68 | 10.28 | **+130.2%** | ✅ |
| 10 | dtsort | 24.59 | 9.69 | **+153.8%** | ✅ |
| 20 | dtsort | 61.07 | 48.70 | **+25.4%** | ✅ |
| 22 | dtsort | 65.77 | 49.44 | **+33.0%** | ✅ |
| 25 | dtsort | 73.53 | 58.59 | **+25.5%** | ✅ |
| 28 | dtsort | 82.16 | 66.10 | **+24.3%** | ✅ |
| 32 | dtsort | 86.95 | 72.53 | **+19.9%** | ✅ |
| 36 | dtsort | 100.22 | 86.41 | **+16.0%** | ✅ |
| 40 | dtsort | 111.17 | 90.63 | **+22.7%** | ✅ |
| 45 | dtsort | 122.55 | 161.01 | -23.9% | ⚠️ |
| 51 | dtsort | 135.94 | 180.10 | -24.5% | ⚠️ |
| 57 | dtsort | 153.78 | 200.76 | -23.4% | ⚠️ |
| 64 | dtsort | 168.16 | 216.80 | -22.4% | ⚠️ |
| 72 | dtsort | 310.81 | 247.05 | **+25.8%** | ✅ |
| 81 | dtsort | 342.43 | 285.80 | **+19.8%** | ✅ |
| 91 | dtsort | 402.18 | 339.80 | **+18.4%** | ✅ |
| 102 | dtsort | 431.50 | 362.65 | **+19.0%** | ✅ |
| 114 | dtsort | 477.25 | 442.61 | **+7.8%** | ✅ |
| 129 | dtsort | 531.10 | 464.95 | **+14.2%** | ✅ |
| 144 | dtsort | 590.78 | 518.95 | **+13.8%** | ✅ |
| 162 | dtsort | 672.40 | 610.42 | **+10.2%** | ✅ |
| 182 | dtsort | 748.48 | 923.36 | -18.9% | ⚠️ |
| 205 | dtsort | 862.53 | 1018.22 | -15.3% | ⚠️ |
| 230 | dtsort | 981.94 | 1167.74 | -15.9% | ⚠️ |
| 258 | dtsort | 1099.39 | 1254.72 | -12.4% | ⚠️ |
| 290 | dtsort | 1690.49 | 1429.90 | **+18.2%** | ✅ |
| 326 | dtsort | 1903.06 | 1693.65 | **+12.4%** | ✅ |
| 366 | dtsort | 2168.24 | 1928.63 | **+12.4%** | ✅ |
| 411 | dtsort | 2410.94 | 2155.26 | **+11.9%** | ✅ |
| 462 | dtsort | 2727.12 | 2465.12 | **+10.6%** | ✅ |
| 519 | dtsort | 2994.20 | 2757.50 | **+8.6%** | ✅ |
| 583 | dtsort | 3361.21 | 3120.84 | **+7.7%** | ✅ |
| 655 | dtsort | 3818.53 | 3727.04 | **+2.5%** | ✅ |
| 736 | dtsort | 4287.30 | 4853.85 | -11.7% | ⚠️ |
| 826 | dtsort | 4971.68 | 5612.52 | -11.4% | ⚠️ |
| 928 | dtsort | 5449.22 | 6144.26 | -11.3% | ⚠️ |
| 1,043 | dtsort | 6428.23 | 6912.86 | -7.0% | ⚠️ |
| 1,171 | dtsort | 8773.30 | 7843.56 | **+11.9%** | ✅ |
| 1,316 | dtsort | 9827.87 | 9003.88 | **+9.2%** | ✅ |
| 1,478 | dtsort | 10991.30 | 10027.30 | **+9.6%** | ✅ |
| 1,660 | dtsort | 12488.00 | 11459.90 | **+9.0%** | ✅ |
| 1,865 | dtsort | 14048.54 | 12984.11 | **+8.2%** | ✅ |
| 2,095 | dtsort | 15907.43 | 15042.16 | **+5.8%** | ✅ |
| 2,354 | dtsort | 17846.80 | 17075.31 | **+4.5%** | ✅ |
| 2,644 | dtsort | 20029.26 | 20377.36 | -1.7% | ⚠️ |
| 2,970 | dtsort | 22312.55 | 24812.94 | -10.1% | ⚠️ |
| 3,336 | dtsort | 25180.79 | 27685.66 | -9.0% | ⚠️ |
| 3,748 | dtsort | 28324.52 | 31073.59 | -8.8% | ⚠️ |
| 4,210 | dtsort | 34344.34 | 35387.97 | -2.9% | ⚠️ |
| 4,729 | dtsort | 43088.87 | 39685.71 | **+8.6%** | ✅ |
| 5,312 | dtsort | 48751.96 | 46227.40 | **+5.5%** | ✅ |
| 5,967 | dtsort | 54441.96 | 51169.86 | **+6.4%** | ✅ |
| 6,703 | dtsort | 61682.79 | 58022.48 | **+6.3%** | ✅ |
| 7,530 | dtsort | 69722.57 | 65690.01 | **+6.1%** | ✅ |
| 8,458 | dtsort | 78606.46 | 74591.84 | **+5.4%** | ✅ |
| 9,502 | dtsort | 88175.96 | 83991.53 | **+5.0%** | ✅ |
| 10,673 | dtsort | 99719.83 | 102591.71 | -2.8% | ⚠️ |
| 11,990 | dtsort | 112249.72 | 120196.30 | -6.6% | ⚠️ |
| 13,468 | dtsort | 124941.92 | 133739.83 | -6.6% | ⚠️ |
| 15,129 | dtsort | 140417.80 | 149398.66 | -6.0% | ⚠️ |
| 16,995 | dtsort | 166526.50 | 167557.70 | -0.6% | ✅ |
| 19,091 | dtsort | 205149.79 | 189939.21 | **+8.0%** | ✅ |
| 21,445 | dtsort | 228345.85 | 218665.23 | **+4.4%** | ✅ |
| 24,090 | dtsort | 257510.16 | 243459.08 | **+5.8%** | ✅ |
| 27,061 | dtsort | 288083.76 | 273530.63 | **+5.3%** | ✅ |
| 30,398 | dtsort | 325177.27 | 310093.31 | **+4.9%** | ✅ |
| 34,147 | dtsort | 366877.48 | 348675.07 | **+5.2%** | ✅ |
| 38,358 | dtsort | 413671.93 | 392945.36 | **+5.3%** | ✅ |
| 43,089 | dtsort | 464456.89 | 471536.28 | -1.5% | ⚠️ |
| 48,403 | dtsort | 524568.70 | 554494.99 | -5.4% | ⚠️ |
| 54,372 | dtsort | 583441.36 | 618950.51 | -5.7% | ⚠️ |
| 61,077 | dtsort | 669744.41 | 698569.89 | -4.1% | ⚠️ |
| 68,609 | dtsort | 789859.77 | 778938.03 | **+1.4%** | ✅ |
| 77,071 | dtsort | 959011.25 | 889315.51 | **+7.8%** | ✅ |
| 86,575 | dtsort | 1046700.25 | 1002799.06 | **+4.4%** | ✅ |
| 97,252 | dtsort | 1195094.50 | 1129039.85 | **+5.9%** | ✅ |
| 109,246 | dtsort | 1332782.50 | 1271986.01 | **+4.8%** | ✅ |
| 122,718 | dtsort | 1478893.13 | 1420526.34 | **+4.1%** | ✅ |
| 137,852 | dtsort | 1699519.57 | 1619812.84 | **+4.9%** | ✅ |
| 154,853 | dtsort | 1919928.83 | 1829026.76 | **+5.0%** | ✅ |
| 173,950 | dtsort | 2131296.88 | 2174418.61 | -2.0% | ⚠️ |
| 195,402 | dtsort | 2414664.22 | 2544009.24 | -5.1% | ⚠️ |
| 219,500 | dtsort | 2720535.99 | 2863600.85 | -5.0% | ⚠️ |
| 246,569 | dtsort | 3032151.51 | 3175715.15 | -4.5% | ⚠️ |
| 276,977 | dtsort | 3695096.56 | 3623580.30 | **+2.0%** | ✅ |
| 311,135 | dtsort | 4329179.78 | 4063047.51 | **+6.6%** | ✅ |
| 349,506 | dtsort | 4822607.48 | 4622726.70 | **+4.3%** | ✅ |
| 392,608 | dtsort | 5436638.99 | 5162898.60 | **+5.3%** | ✅ |
| 441,026 | dtsort | 6083253.26 | 5846300.00 | **+4.1%** | ✅ |
| 495,415 | dtsort | 6901020.21 | 6594231.52 | **+4.7%** | ✅ |
| 556,512 | dtsort | 7673223.90 | 7357369.74 | **+4.3%** | ✅ |
| 625,143 | dtsort | 8659429.01 | 8298261.40 | **+4.4%** | ✅ |
| 702,238 | dtsort | 9742890.63 | 10034192.86 | -2.9% | ⚠️ |
| 788,841 | dtsort | 10912268.23 | 11451114.76 | -4.7% | ⚠️ |
| 886,124 | dtsort | 12195836.25 | 12828102.28 | -4.9% | ⚠️ |
| 995,405 | dtsort | 13836501.63 | 14446055.26 | -4.2% | ⚠️ |
| 1,118,162 | dtsort | 16667971.24 | 16234098.84 | **+2.7%** | ✅ |
| 1,256,058 | dtsort | 19266120.38 | 18343096.50 | **+5.0%** | ✅ |
| 1,410,960 | dtsort | 21505309.90 | 20766626.24 | **+3.6%** | ✅ |
| 1,584,966 | dtsort | 24391195.41 | 23209800.00 | **+5.1%** | ✅ |
| 1,780,430 | dtsort | 27273738.77 | 26318675.92 | **+3.6%** | ✅ |
| 2,000,000 | dtsort | 30669311.61 | 29644843.74 | **+3.5%** | ✅ |

#### LLVM19 (WideRow)
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 210.81 | 189.86 | **+11.0%** | ✅ |
| 3 | dtsort | 363.09 | 265.66 | **+36.7%** | ✅ |
| 4 | dtsort | 494.41 | 349.08 | **+41.6%** | ✅ |
| 5 | dtsort | 677.68 | 460.17 | **+47.3%** | ✅ |
| 6 | dtsort | 766.12 | 793.53 | -3.5% | ⚠️ |
| 7 | dtsort | 884.57 | 780.92 | **+13.3%** | ✅ |
| 8 | dtsort | 1309.84 | 881.36 | **+48.6%** | ✅ |
| 9 | dtsort | 1752.04 | 998.73 | **+75.4%** | ✅ |
| 10 | dtsort | 2002.94 | 1224.72 | **+63.5%** | ✅ |
| 20 | dtsort | 6982.84 | 4732.25 | **+47.6%** | ✅ |
| 22 | dtsort | 7996.16 | 6666.23 | **+20.0%** | ✅ |
| 25 | dtsort | 9379.63 | 8112.91 | **+15.6%** | ✅ |
| 28 | dtsort | 10669.97 | 9227.25 | **+15.6%** | ✅ |
| 32 | dtsort | 12467.98 | 10394.96 | **+19.9%** | ✅ |
| 36 | dtsort | 13851.38 | 12029.41 | **+15.1%** | ✅ |
| 40 | dtsort | 16197.91 | 13348.49 | **+21.3%** | ✅ |
| 45 | dtsort | 18954.25 | 16620.12 | **+14.0%** | ✅ |
| 51 | dtsort | 21976.90 | 19068.26 | **+15.3%** | ✅ |
| 57 | dtsort | 24453.23 | 21477.42 | **+13.9%** | ✅ |
| 64 | dtsort | 27939.76 | 24001.35 | **+16.4%** | ✅ |
| 72 | dtsort | 33951.46 | 27022.35 | **+25.6%** | ✅ |
| 81 | dtsort | 39314.41 | 31808.78 | **+23.6%** | ✅ |
| 91 | dtsort | 45126.72 | 41385.41 | **+9.0%** | ✅ |
| 102 | dtsort | 52036.20 | 48325.66 | **+7.7%** | ✅ |
| 114 | dtsort | 58587.34 | 53243.33 | **+10.0%** | ✅ |
| 129 | dtsort | 68402.18 | 62492.21 | **+9.5%** | ✅ |
| 144 | dtsort | 77514.49 | 68750.54 | **+12.7%** | ✅ |
| 162 | dtsort | 89874.95 | 79060.05 | **+13.7%** | ✅ |
| 182 | dtsort | 102086.50 | 94506.03 | **+8.0%** | ✅ |
| 205 | dtsort | 117972.61 | 107382.56 | **+9.9%** | ✅ |
| 230 | dtsort | 135934.79 | 121089.91 | **+12.3%** | ✅ |
| 258 | dtsort | 155991.94 | 136362.46 | **+14.4%** | ✅ |
| 290 | dtsort | 182922.34 | 155579.50 | **+17.6%** | ✅ |
| 326 | dtsort | 209433.39 | 178936.39 | **+17.0%** | ✅ |
| 366 | dtsort | 239389.19 | 225077.51 | **+6.4%** | ✅ |
| 411 | dtsort | 274867.84 | 257079.14 | **+6.9%** | ✅ |
| 462 | dtsort | 312988.89 | 289293.95 | **+8.2%** | ✅ |
| 519 | dtsort | 363448.57 | 329595.17 | **+10.3%** | ✅ |
| 583 | dtsort | 427060.50 | 375185.82 | **+13.8%** | ✅ |
| 655 | dtsort | 475211.37 | 430824.91 | **+10.3%** | ✅ |
| 736 | dtsort | 557335.38 | 529176.01 | **+5.3%** | ✅ |
| 826 | dtsort | 613574.93 | 571673.46 | **+7.3%** | ✅ |
| 928 | dtsort | 717628.76 | 662452.82 | **+8.3%** | ✅ |
| 1,043 | dtsort | 802080.59 | 718746.08 | **+11.6%** | ✅ |
| 1,171 | dtsort | 940526.50 | 824640.74 | **+14.1%** | ✅ |
| 1,316 | dtsort | 1075317.13 | 953640.25 | **+12.8%** | ✅ |
| 1,478 | dtsort | 1229949.45 | 1169038.33 | **+5.2%** | ✅ |
| 1,660 | dtsort | 1413503.21 | 1317885.34 | **+7.3%** | ✅ |
| 1,865 | dtsort | 1600021.46 | 1497374.10 | **+6.9%** | ✅ |
| 2,095 | dtsort | 1821643.52 | 1697473.58 | **+7.3%** | ✅ |
| 2,354 | dtsort | 2105376.39 | 1932637.68 | **+8.9%** | ✅ |
| 2,644 | dtsort | 2384141.61 | 2235785.35 | **+6.6%** | ✅ |
| 2,970 | dtsort | 2744689.16 | 2644588.48 | **+3.8%** | ✅ |
| 3,336 | dtsort | 3176274.73 | 3023383.81 | **+5.1%** | ✅ |
| 3,748 | dtsort | 3677165.33 | 3521447.19 | **+4.4%** | ✅ |
| 4,210 | dtsort | 4325269.29 | 4096399.41 | **+5.6%** | ✅ |
| 4,729 | dtsort | 5008779.71 | 4793774.09 | **+4.5%** | ✅ |
| 5,312 | dtsort | 5871520.47 | 5665213.84 | **+3.6%** | ✅ |
| 5,967 | dtsort | 6811669.94 | 6529492.32 | **+4.3%** | ✅ |
| 6,703 | dtsort | 7940735.32 | 7573152.02 | **+4.9%** | ✅ |
| 7,530 | dtsort | 9102476.42 | 8708648.20 | **+4.5%** | ✅ |
| 8,458 | dtsort | 10557817.17 | 10087792.28 | **+4.7%** | ✅ |
| 9,502 | dtsort | 12231111.12 | 11623041.67 | **+5.2%** | ✅ |
| 10,673 | dtsort | 14087166.66 | 13820179.75 | **+1.9%** | ✅ |
| 11,990 | dtsort | 16144539.74 | 16123820.06 | +0.1% | ✅ |
| 13,468 | dtsort | 18474796.05 | 18391263.16 | +0.5% | ✅ |
| 15,129 | dtsort | 21333508.85 | 21067638.87 | **+1.3%** | ✅ |
| 16,995 | dtsort | 24599983.65 | 24058054.59 | **+2.3%** | ✅ |
| 19,091 | dtsort | 28319811.68 | 27452551.31 | **+3.2%** | ✅ |
| 21,445 | dtsort | 32536410.70 | 31535645.82 | **+3.2%** | ✅ |
| 24,090 | dtsort | 37151103.06 | 36485337.69 | **+1.8%** | ✅ |
| 27,061 | dtsort | 42645529.42 | 41527799.00 | **+2.7%** | ✅ |
| 30,398 | dtsort | 48020455.35 | 46988077.80 | **+2.2%** | ✅ |
| 34,147 | dtsort | 54256694.43 | 53240487.16 | **+1.9%** | ✅ |
| 38,358 | dtsort | 61385104.20 | 60305128.72 | **+1.8%** | ✅ |
| 43,089 | dtsort | 69640088.00 | 70003108.31 | -0.5% | ✅ |
| 48,403 | dtsort | 78920880.13 | 79618255.26 | -0.9% | ✅ |
| 54,372 | dtsort | 89364791.72 | 90550779.70 | -1.3% | ⚠️ |
| 61,077 | dtsort | 101513062.49 | 102563986.01 | -1.0% | ⚠️ |
| 68,609 | dtsort | 116279958.32 | 116359340.34 | -0.1% | ✅ |
| 77,071 | dtsort | 134132591.82 | 131620049.99 | **+1.9%** | ✅ |
| 86,575 | dtsort | 152939843.77 | 150714437.54 | **+1.5%** | ✅ |
| 97,252 | dtsort | 174438593.73 | 172198624.99 | **+1.3%** | ✅ |
| 109,246 | dtsort | 198993527.72 | 196298444.64 | **+1.4%** | ✅ |
| 122,718 | dtsort | 225260986.33 | 222551736.28 | **+1.2%** | ✅ |
| 137,852 | dtsort | 255701499.99 | 252932645.96 | **+1.1%** | ✅ |
| 154,853 | dtsort | 289773729.51 | 285878625.00 | **+1.4%** | ✅ |
| 173,950 | dtsort | 329234479.46 | 331201917.03 | -0.6% | ✅ |
| 195,402 | dtsort | 373500062.50 | 378284958.07 | -1.3% | ⚠️ |
| 219,500 | dtsort | 423226583.40 | 426883875.04 | -0.9% | ✅ |
| 246,569 | dtsort | 479408249.96 | 483041833.85 | -0.8% | ✅ |
| 276,977 | dtsort | 548770250.05 | 547640291.04 | +0.2% | ✅ |
| 311,135 | dtsort | 628852833.06 | 618775291.83 | **+1.6%** | ✅ |
| 349,506 | dtsort | 716275791.17 | 708076874.96 | **+1.2%** | ✅ |
| 392,608 | dtsort | 814152959.04 | 804458375.09 | **+1.2%** | ✅ |
| 441,026 | dtsort | 927381834.02 | 915100874.84 | **+1.3%** | ✅ |
| 495,415 | dtsort | 1051467415.88 | 1038063333.14 | **+1.3%** | ✅ |
| 556,512 | dtsort | 1189286082.98 | 1178489207.99 | **+0.9%** | ✅ |
| 625,143 | dtsort | 1345652499.93 | 1332922125.00 | **+1.0%** | ✅ |
| 702,238 | dtsort | 1526667749.97 | 1537581749.96 | -0.7% | ✅ |
| 788,841 | dtsort | 1735823415.92 | 1740562707.88 | -0.3% | ✅ |
| 886,124 | dtsort | 1956441083.92 | 1971843042.18 | -0.8% | ✅ |
| 995,405 | dtsort | 2218255665.85 | 2236288083.95 | -0.8% | ✅ |
| 1,118,162 | dtsort | 2808498749.98 | 2760844874.89 | **+1.7%** | ✅ |
| 1,256,058 | dtsort | 3221831707.98 | 3160203458.04 | **+2.0%** | ✅ |
| 1,410,960 | dtsort | 3654804458.14 | 3596906166.99 | **+1.6%** | ✅ |
| 1,584,966 | dtsort | 4150502166.94 | 4071406041.97 | **+1.9%** | ✅ |
| 1,780,430 | dtsort | 4698133416.94 | 4621818582.99 | **+1.7%** | ✅ |
| 2,000,000 | dtsort | 5320190208.96 | 5221801958.98 | **+1.9%** | ✅ |

#### LLVM19 (WideRow (Reverse))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 210.22 | 190.09 | **+10.6%** | ✅ |
| 3 | dtsort | 363.44 | 265.69 | **+36.8%** | ✅ |
| 4 | dtsort | 484.57 | 360.51 | **+34.4%** | ✅ |
| 5 | dtsort | 720.53 | 397.08 | **+81.5%** | ✅ |
| 6 | dtsort | 891.16 | 730.30 | **+22.0%** | ✅ |
| 7 | dtsort | 1147.78 | 840.49 | **+36.6%** | ✅ |
| 8 | dtsort | 1416.14 | 1110.97 | **+27.5%** | ✅ |
| 9 | dtsort | 1842.02 | 1216.11 | **+51.5%** | ✅ |
| 10 | dtsort | 2176.61 | 1442.78 | **+50.9%** | ✅ |
| 20 | dtsort | 7061.97 | 4804.53 | **+47.0%** | ✅ |
| 22 | dtsort | 8139.47 | 7410.81 | **+9.8%** | ✅ |
| 25 | dtsort | 9348.04 | 8776.10 | **+6.5%** | ✅ |
| 28 | dtsort | 10658.13 | 9511.33 | **+12.1%** | ✅ |
| 32 | dtsort | 12422.94 | 11158.84 | **+11.3%** | ✅ |
| 36 | dtsort | 14949.53 | 12881.79 | **+16.1%** | ✅ |
| 40 | dtsort | 16811.78 | 14585.84 | **+15.3%** | ✅ |
| 45 | dtsort | 20126.55 | 17017.93 | **+18.3%** | ✅ |
| 51 | dtsort | 23949.38 | 19415.10 | **+23.4%** | ✅ |
| 57 | dtsort | 27541.51 | 21700.58 | **+26.9%** | ✅ |
| 64 | dtsort | 31564.42 | 24266.20 | **+30.1%** | ✅ |
| 72 | dtsort | 33933.18 | 27500.94 | **+23.4%** | ✅ |
| 81 | dtsort | 39348.26 | 32003.90 | **+22.9%** | ✅ |
| 91 | dtsort | 46285.54 | 44694.10 | **+3.6%** | ✅ |
| 102 | dtsort | 53627.35 | 50920.44 | **+5.3%** | ✅ |
| 114 | dtsort | 61105.06 | 57522.27 | **+6.2%** | ✅ |
| 129 | dtsort | 69737.34 | 63384.22 | **+10.0%** | ✅ |
| 144 | dtsort | 79586.20 | 71190.82 | **+11.8%** | ✅ |
| 162 | dtsort | 93577.46 | 83021.14 | **+12.7%** | ✅ |
| 182 | dtsort | 108093.47 | 96219.54 | **+12.3%** | ✅ |
| 205 | dtsort | 126169.10 | 108070.86 | **+16.7%** | ✅ |
| 230 | dtsort | 145611.64 | 123057.72 | **+18.3%** | ✅ |
| 258 | dtsort | 166948.12 | 138441.95 | **+20.6%** | ✅ |
| 290 | dtsort | 182316.17 | 154994.90 | **+17.6%** | ✅ |
| 326 | dtsort | 210988.42 | 182698.55 | **+15.5%** | ✅ |
| 366 | dtsort | 242000.76 | 235165.98 | **+2.9%** | ✅ |
| 411 | dtsort | 278760.73 | 268238.71 | **+3.9%** | ✅ |
| 462 | dtsort | 319102.47 | 301509.78 | **+5.8%** | ✅ |
| 519 | dtsort | 374793.98 | 337150.41 | **+11.2%** | ✅ |
| 583 | dtsort | 426271.84 | 384206.43 | **+10.9%** | ✅ |
| 655 | dtsort | 483521.64 | 440275.15 | **+9.8%** | ✅ |
| 736 | dtsort | 554456.04 | 504267.21 | **+10.0%** | ✅ |
| 826 | dtsort | 639287.26 | 568172.00 | **+12.5%** | ✅ |
| 928 | dtsort | 734750.31 | 645243.89 | **+13.9%** | ✅ |
| 1,043 | dtsort | 836628.86 | 726671.50 | **+15.1%** | ✅ |
| 1,171 | dtsort | 938887.12 | 825828.49 | **+13.7%** | ✅ |
| 1,316 | dtsort | 1071117.72 | 964005.52 | **+11.1%** | ✅ |
| 1,478 | dtsort | 1238200.54 | 1195266.72 | **+3.6%** | ✅ |
| 1,660 | dtsort | 1405589.92 | 1354841.53 | **+3.7%** | ✅ |
| 1,865 | dtsort | 1665371.54 | 1567333.33 | **+6.3%** | ✅ |
| 2,095 | dtsort | 1880474.64 | 1723120.36 | **+9.1%** | ✅ |
| 2,354 | dtsort | 2126894.17 | 1954414.74 | **+8.8%** | ✅ |
| 2,644 | dtsort | 2449672.21 | 2265657.22 | **+8.1%** | ✅ |
| 2,970 | dtsort | 2868053.18 | 2645818.92 | **+8.4%** | ✅ |
| 3,336 | dtsort | 3281778.57 | 3049633.12 | **+7.6%** | ✅ |
| 3,748 | dtsort | 3866138.51 | 3557357.05 | **+8.7%** | ✅ |
| 4,210 | dtsort | 4515906.59 | 4208485.94 | **+7.3%** | ✅ |
| 4,729 | dtsort | 5083240.91 | 4931458.04 | **+3.1%** | ✅ |
| 5,312 | dtsort | 5920433.41 | 5817413.85 | **+1.8%** | ✅ |
| 5,967 | dtsort | 6948837.97 | 6877443.18 | **+1.0%** | ✅ |
| 6,703 | dtsort | 8047335.78 | 7922103.69 | **+1.6%** | ✅ |
| 7,530 | dtsort | 9339492.59 | 9204925.56 | **+1.5%** | ✅ |
| 8,458 | dtsort | 10767640.38 | 10567607.32 | **+1.9%** | ✅ |
| 9,502 | dtsort | 12483284.23 | 12193094.50 | **+2.4%** | ✅ |
| 10,673 | dtsort | 14384107.64 | 14371942.16 | +0.1% | ✅ |
| 11,990 | dtsort | 16737788.69 | 16761895.83 | -0.1% | ✅ |
| 13,468 | dtsort | 19236423.61 | 19115642.37 | **+0.6%** | ✅ |
| 15,129 | dtsort | 22284666.68 | 21961283.84 | **+1.5%** | ✅ |
| 16,995 | dtsort | 25507844.10 | 24981691.97 | **+2.1%** | ✅ |
| 19,091 | dtsort | 29154977.42 | 28616690.96 | **+1.9%** | ✅ |
| 21,445 | dtsort | 33316323.43 | 32974017.86 | **+1.0%** | ✅ |
| 24,090 | dtsort | 37955895.83 | 38082280.11 | -0.3% | ✅ |
| 27,061 | dtsort | 43599502.62 | 43438361.93 | +0.4% | ✅ |
| 30,398 | dtsort | 49290272.46 | 49107737.16 | +0.4% | ✅ |
| 34,147 | dtsort | 56029011.37 | 55841763.91 | +0.3% | ✅ |
| 38,358 | dtsort | 63775845.90 | 63418808.30 | **+0.6%** | ✅ |
| 43,089 | dtsort | 71798013.89 | 72857810.11 | -1.5% | ⚠️ |
| 48,403 | dtsort | 82072364.60 | 82717317.74 | -0.8% | ✅ |
| 54,372 | dtsort | 93263315.44 | 94019309.58 | -0.8% | ✅ |
| 61,077 | dtsort | 106843479.17 | 107312562.53 | -0.4% | ✅ |
| 68,609 | dtsort | 120273675.02 | 120041833.17 | +0.2% | ✅ |
| 77,071 | dtsort | 138874406.29 | 137147800.02 | **+1.3%** | ✅ |
| 86,575 | dtsort | 157403323.04 | 157475583.26 | -0.0% | ✅ |
| 97,252 | dtsort | 179208197.98 | 179974260.22 | -0.4% | ✅ |
| 109,246 | dtsort | 205154472.03 | 206085277.95 | -0.5% | ✅ |
| 122,718 | dtsort | 233819625.03 | 237018125.05 | -1.3% | ⚠️ |
| 137,852 | dtsort | 266246937.56 | 265836499.98 | +0.2% | ✅ |
| 154,853 | dtsort | 301891791.98 | 301242458.52 | +0.2% | ✅ |
| 173,950 | dtsort | 341622666.57 | 346333020.48 | -1.4% | ⚠️ |
| 195,402 | dtsort | 389712187.58 | 393139978.99 | -0.9% | ✅ |
| 219,500 | dtsort | 445154395.42 | 446503666.46 | -0.3% | ✅ |
| 246,569 | dtsort | 505788958.17 | 506680415.94 | -0.2% | ✅ |
| 276,977 | dtsort | 574173165.95 | 573710667.20 | +0.1% | ✅ |
| 311,135 | dtsort | 648278167.01 | 652557124.85 | -0.7% | ✅ |
| 349,506 | dtsort | 745542208.90 | 745324458.93 | +0.0% | ✅ |
| 392,608 | dtsort | 848231832.96 | 849108541.85 | -0.1% | ✅ |
| 441,026 | dtsort | 960548459.09 | 966713791.01 | -0.6% | ✅ |
| 495,415 | dtsort | 1093638250.14 | 1092986583.01 | +0.1% | ✅ |
| 556,512 | dtsort | 1232293375.07 | 1252946583.09 | -1.6% | ⚠️ |
| 625,143 | dtsort | 1401910917.13 | 1399527125.06 | +0.2% | ✅ |
| 702,238 | dtsort | 1598498374.92 | 1607244249.90 | -0.5% | ✅ |
| 788,841 | dtsort | 1803027957.91 | 1827889041.98 | -1.4% | ⚠️ |
| 886,124 | dtsort | 2051834332.99 | 2061198582.87 | -0.5% | ✅ |
| 995,405 | dtsort | 2336233749.99 | 2340049166.93 | -0.2% | ✅ |
| 1,118,162 | dtsort | 2875945375.07 | 2834959290.92 | **+1.4%** | ✅ |
| 1,256,058 | dtsort | 3305582582.95 | 3252870832.80 | **+1.6%** | ✅ |
| 1,410,960 | dtsort | 3743208833.97 | 3707635833.88 | **+1.0%** | ✅ |
| 1,584,966 | dtsort | 4272224041.87 | 4237494207.92 | **+0.8%** | ✅ |
| 1,780,430 | dtsort | 4870249625.06 | 4828167499.97 | **+0.9%** | ✅ |
| 2,000,000 | dtsort | 5486686167.07 | 5427415999.82 | **+1.1%** | ✅ |

#### LLVM19 (WideRow (Sorted))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort | 100.51 | 86.49 | **+16.2%** | ✅ |
| 3 | dtsort | 362.84 | 119.20 | **+204.4%** | ✅ |
| 4 | dtsort | 478.97 | 168.22 | **+184.7%** | ✅ |
| 5 | dtsort | 600.26 | 211.19 | **+184.2%** | ✅ |
| 6 | dtsort | 720.65 | 263.87 | **+173.1%** | ✅ |
| 7 | dtsort | 836.64 | 310.17 | **+169.7%** | ✅ |
| 8 | dtsort | 1060.44 | 382.18 | **+177.5%** | ✅ |
| 9 | dtsort | 1307.93 | 420.43 | **+211.1%** | ✅ |
| 10 | dtsort | 1622.23 | 464.20 | **+249.5%** | ✅ |
| 20 | dtsort | 6776.84 | 4712.40 | **+43.8%** | ✅ |
| 22 | dtsort | 7584.13 | 5115.20 | **+48.3%** | ✅ |
| 25 | dtsort | 8725.99 | 5952.88 | **+46.6%** | ✅ |
| 28 | dtsort | 9837.42 | 6586.22 | **+49.4%** | ✅ |
| 32 | dtsort | 11412.43 | 7510.84 | **+51.9%** | ✅ |
| 36 | dtsort | 12996.36 | 8426.12 | **+54.2%** | ✅ |
| 40 | dtsort | 14677.23 | 9338.48 | **+57.2%** | ✅ |
| 45 | dtsort | 16667.87 | 16683.73 | -0.1% | ✅ |
| 51 | dtsort | 18831.48 | 18790.16 | +0.2% | ✅ |
| 57 | dtsort | 21193.55 | 21312.86 | -0.6% | ✅ |
| 64 | dtsort | 23791.20 | 23850.14 | -0.2% | ✅ |
| 72 | dtsort | 33860.22 | 27149.81 | **+24.7%** | ✅ |
| 81 | dtsort | 39041.80 | 30785.68 | **+26.8%** | ✅ |
| 91 | dtsort | 44425.48 | 34920.42 | **+27.2%** | ✅ |
| 102 | dtsort | 50161.35 | 39221.01 | **+27.9%** | ✅ |
| 114 | dtsort | 56930.49 | 43611.88 | **+30.5%** | ✅ |
| 129 | dtsort | 64578.13 | 48639.32 | **+32.8%** | ✅ |
| 144 | dtsort | 73014.14 | 55178.67 | **+32.3%** | ✅ |
| 162 | dtsort | 82476.11 | 63837.51 | **+29.2%** | ✅ |
| 182 | dtsort | 92346.86 | 93979.26 | -1.7% | ⚠️ |
| 205 | dtsort | 105098.26 | 105669.50 | -0.5% | ✅ |
| 230 | dtsort | 118419.30 | 118499.92 | -0.1% | ✅ |
| 258 | dtsort | 136115.69 | 135396.28 | **+0.5%** | ✅ |
| 290 | dtsort | 178610.46 | 152025.54 | **+17.5%** | ✅ |
| 326 | dtsort | 204325.32 | 170369.04 | **+19.9%** | ✅ |
| 366 | dtsort | 231769.26 | 193784.98 | **+19.6%** | ✅ |
| 411 | dtsort | 265113.92 | 217928.97 | **+21.7%** | ✅ |
| 462 | dtsort | 298268.44 | 242878.50 | **+22.8%** | ✅ |
| 519 | dtsort | 341447.33 | 274187.76 | **+24.5%** | ✅ |
| 583 | dtsort | 387443.24 | 309191.42 | **+25.3%** | ✅ |
| 655 | dtsort | 435026.14 | 370107.89 | **+17.5%** | ✅ |
| 736 | dtsort | 489708.57 | 491022.92 | -0.3% | ✅ |
| 826 | dtsort | 549356.77 | 550765.36 | -0.3% | ✅ |
| 928 | dtsort | 620938.74 | 624324.67 | -0.5% | ✅ |
| 1,043 | dtsort | 717677.34 | 702127.37 | **+2.2%** | ✅ |
| 1,171 | dtsort | 909854.55 | 798578.69 | **+13.9%** | ✅ |
| 1,316 | dtsort | 1047607.75 | 902310.34 | **+16.1%** | ✅ |
| 1,478 | dtsort | 1179408.77 | 1014784.47 | **+16.2%** | ✅ |
| 1,660 | dtsort | 1332570.33 | 1146638.24 | **+16.2%** | ✅ |
| 1,865 | dtsort | 1516176.06 | 1287091.41 | **+17.8%** | ✅ |
| 2,095 | dtsort | 1718886.44 | 1462230.99 | **+17.6%** | ✅ |
| 2,354 | dtsort | 1966904.13 | 1651740.97 | **+19.1%** | ✅ |
| 2,644 | dtsort | 2227625.94 | 2048577.31 | **+8.7%** | ✅ |
| 2,970 | dtsort | 2564869.27 | 2572745.91 | -0.3% | ✅ |
| 3,336 | dtsort | 2966375.18 | 2984570.64 | -0.6% | ✅ |
| 3,748 | dtsort | 3424827.99 | 3441851.58 | -0.5% | ✅ |
| 4,210 | dtsort | 4091831.13 | 4066438.70 | **+0.6%** | ✅ |
| 4,729 | dtsort | 4956232.72 | 4685911.63 | **+5.8%** | ✅ |
| 5,312 | dtsort | 5940909.55 | 5427071.85 | **+9.5%** | ✅ |
| 5,967 | dtsort | 6846803.51 | 6262386.26 | **+9.3%** | ✅ |
| 6,703 | dtsort | 7937000.00 | 7249536.40 | **+9.5%** | ✅ |
| 7,530 | dtsort | 9177968.75 | 8355644.84 | **+9.8%** | ✅ |
| 8,458 | dtsort | 10499224.12 | 9573865.86 | **+9.7%** | ✅ |
| 9,502 | dtsort | 12119490.34 | 10979599.86 | **+10.4%** | ✅ |
| 10,673 | dtsort | 13829520.00 | 13492909.46 | **+2.5%** | ✅ |
| 11,990 | dtsort | 15841973.50 | 15875144.89 | -0.2% | ✅ |
| 13,468 | dtsort | 18105291.66 | 18194540.59 | -0.5% | ✅ |
| 15,129 | dtsort | 20727445.70 | 20883786.64 | -0.7% | ✅ |
| 16,995 | dtsort | 23979091.93 | 23902321.43 | +0.3% | ✅ |
| 19,091 | dtsort | 28404883.32 | 27095943.89 | **+4.8%** | ✅ |
| 21,445 | dtsort | 32458856.09 | 30711996.39 | **+5.7%** | ✅ |
| 24,090 | dtsort | 36999368.42 | 34788093.75 | **+6.4%** | ✅ |
| 27,061 | dtsort | 42001416.65 | 39440974.50 | **+6.5%** | ✅ |
| 30,398 | dtsort | 47603488.08 | 44554927.12 | **+6.8%** | ✅ |
| 34,147 | dtsort | 54038406.25 | 50244522.39 | **+7.6%** | ✅ |
| 38,358 | dtsort | 60893833.36 | 56556572.90 | **+7.7%** | ✅ |
| 43,089 | dtsort | 68950703.68 | 68401800.91 | **+0.8%** | ✅ |
| 48,403 | dtsort | 78031203.13 | 78708583.26 | -0.9% | ✅ |
| 54,372 | dtsort | 88425601.13 | 89419696.41 | -1.1% | ⚠️ |
| 61,077 | dtsort | 100160993.01 | 102300659.68 | -2.1% | ⚠️ |
| 68,609 | dtsort | 115545574.99 | 114940991.63 | **+0.5%** | ✅ |
| 77,071 | dtsort | 135356675.00 | 129945316.58 | **+4.2%** | ✅ |
| 86,575 | dtsort | 154683458.21 | 146759271.04 | **+5.4%** | ✅ |
| 97,252 | dtsort | 175525364.76 | 165946291.76 | **+5.8%** | ✅ |
| 109,246 | dtsort | 198721027.93 | 187794485.98 | **+5.8%** | ✅ |
| 122,718 | dtsort | 227592680.68 | 212439249.97 | **+7.1%** | ✅ |
| 137,852 | dtsort | 255182604.07 | 240099653.06 | **+6.3%** | ✅ |
| 154,853 | dtsort | 289305333.05 | 271794604.02 | **+6.4%** | ✅ |
| 173,950 | dtsort | 327862333.51 | 326679979.45 | +0.4% | ✅ |
| 195,402 | dtsort | 370704000.00 | 376945625.06 | -1.7% | ⚠️ |
| 219,500 | dtsort | 419569916.56 | 423578187.59 | -0.9% | ✅ |
| 246,569 | dtsort | 475202375.09 | 479518915.99 | -0.9% | ✅ |
| 276,977 | dtsort | 548610707.98 | 546023249.86 | +0.5% | ✅ |
| 311,135 | dtsort | 638223082.99 | 613104792.08 | **+4.1%** | ✅ |
| 349,506 | dtsort | 725009750.11 | 691223999.95 | **+4.9%** | ✅ |
| 392,608 | dtsort | 820446375.06 | 778485292.10 | **+5.4%** | ✅ |
| 441,026 | dtsort | 930687875.02 | 895603417.19 | **+3.9%** | ✅ |
| 495,415 | dtsort | 1052602417.07 | 1000754792.01 | **+5.2%** | ✅ |
| 556,512 | dtsort | 1189745750.05 | 1130700417.10 | **+5.2%** | ✅ |
| 625,143 | dtsort | 1350129917.03 | 1277377458.06 | **+5.7%** | ✅ |
| 702,238 | dtsort | 1527153708.04 | 1526452374.88 | +0.0% | ✅ |
| 788,841 | dtsort | 1722086332.97 | 1737684916.01 | -0.9% | ✅ |
| 886,124 | dtsort | 1949807624.80 | 1960699792.02 | -0.6% | ✅ |
| 995,405 | dtsort | 2202891834.08 | 2223810540.98 | -0.9% | ✅ |
| 1,118,162 | dtsort | 2775066709.15 | 2723003000.02 | **+1.9%** | ✅ |
| 1,256,058 | dtsort | 3240623457.82 | 3106657332.97 | **+4.3%** | ✅ |
| 1,410,960 | dtsort | 3675030000.04 | 3525953291.92 | **+4.2%** | ✅ |
| 1,584,966 | dtsort | 4163573083.00 | 3973908792.02 | **+4.8%** | ✅ |
| 1,780,430 | dtsort | 4684282332.89 | 4491359666.92 | **+4.3%** | ✅ |
| 2,000,000 | dtsort | 5311086624.86 | 5068123249.80 | **+4.8%** | ✅ |

#### dt (IntNaked)
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort-am | 12.89 | 4.45 | **+189.7%** | ✅ |
| 2 | dtsort-em | 12.89 | 3.22 | **+300.4%** | ✅ |
| 3 | dtsort-am | 18.77 | 5.75 | **+226.3%** | ✅ |
| 3 | dtsort-em | 18.77 | 4.10 | **+358.3%** | ✅ |
| 4 | dtsort-am | 19.50 | 6.85 | **+184.8%** | ✅ |
| 4 | dtsort-em | 19.50 | 4.70 | **+315.3%** | ✅ |
| 5 | dtsort-am | 22.32 | 6.86 | **+225.4%** | ✅ |
| 5 | dtsort-em | 22.32 | 5.00 | **+346.6%** | ✅ |
| 6 | dtsort-am | 22.94 | 7.63 | **+200.5%** | ✅ |
| 6 | dtsort-em | 22.94 | 6.94 | **+230.5%** | ✅ |
| 7 | dtsort-am | 22.73 | 10.37 | **+119.1%** | ✅ |
| 7 | dtsort-em | 22.73 | 7.89 | **+188.0%** | ✅ |
| 8 | dtsort-am | 24.37 | 11.12 | **+119.2%** | ✅ |
| 8 | dtsort-em | 24.37 | 8.75 | **+178.3%** | ✅ |
| 9 | dtsort-am | 26.64 | 13.13 | **+102.9%** | ✅ |
| 9 | dtsort-em | 26.64 | 9.18 | **+190.4%** | ✅ |
| 10 | dtsort-am | 27.96 | 13.86 | **+101.7%** | ✅ |
| 10 | dtsort-em | 27.96 | 10.93 | **+155.7%** | ✅ |
| 20 | dtsort-am | 82.05 | 56.79 | **+44.5%** | ✅ |
| 20 | dtsort-em | 82.05 | 55.75 | **+47.2%** | ✅ |
| 22 | dtsort-am | 92.85 | 78.21 | **+18.7%** | ✅ |
| 22 | dtsort-em | 92.85 | 73.57 | **+26.2%** | ✅ |
| 25 | dtsort-am | 102.68 | 87.34 | **+17.6%** | ✅ |
| 25 | dtsort-em | 102.68 | 88.45 | **+16.1%** | ✅ |
| 28 | dtsort-am | 113.65 | 95.70 | **+18.8%** | ✅ |
| 28 | dtsort-em | 113.65 | 98.81 | **+15.0%** | ✅ |
| 32 | dtsort-am | 135.29 | 123.65 | **+9.4%** | ✅ |
| 32 | dtsort-em | 135.29 | 122.51 | **+10.4%** | ✅ |
| 36 | dtsort-am | 159.28 | 148.30 | **+7.4%** | ✅ |
| 36 | dtsort-em | 159.28 | 152.01 | **+4.8%** | ✅ |
| 40 | dtsort-am | 188.13 | 153.04 | **+22.9%** | ✅ |
| 40 | dtsort-em | 188.13 | 149.21 | **+26.1%** | ✅ |
| 45 | dtsort-am | 213.01 | 194.44 | **+9.5%** | ✅ |
| 45 | dtsort-em | 213.01 | 192.60 | **+10.6%** | ✅ |
| 51 | dtsort-am | 246.20 | 227.13 | **+8.4%** | ✅ |
| 51 | dtsort-em | 246.20 | 228.67 | **+7.7%** | ✅ |
| 57 | dtsort-am | 281.47 | 269.20 | **+4.6%** | ✅ |
| 57 | dtsort-em | 281.47 | 270.81 | **+3.9%** | ✅ |
| 64 | dtsort-am | 322.46 | 326.15 | -1.1% | ⚠️ |
| 64 | dtsort-em | 322.46 | 327.76 | -1.6% | ⚠️ |
| 72 | dtsort-am | 466.68 | 373.00 | **+25.1%** | ✅ |
| 72 | dtsort-em | 466.68 | 376.25 | **+24.0%** | ✅ |
| 81 | dtsort-am | 546.58 | 448.17 | **+22.0%** | ✅ |
| 81 | dtsort-em | 546.58 | 454.49 | **+20.3%** | ✅ |
| 91 | dtsort-am | 646.87 | 534.76 | **+21.0%** | ✅ |
| 91 | dtsort-em | 646.87 | 536.99 | **+20.5%** | ✅ |
| 102 | dtsort-am | 725.51 | 620.71 | **+16.9%** | ✅ |
| 102 | dtsort-em | 725.51 | 619.40 | **+17.1%** | ✅ |
| 114 | dtsort-am | 827.38 | 717.70 | **+15.3%** | ✅ |
| 114 | dtsort-em | 827.38 | 726.31 | **+13.9%** | ✅ |
| 129 | dtsort-am | 961.20 | 860.21 | **+11.7%** | ✅ |
| 129 | dtsort-em | 961.20 | 859.03 | **+11.9%** | ✅ |
| 144 | dtsort-am | 1060.17 | 967.76 | **+9.5%** | ✅ |
| 144 | dtsort-em | 1060.17 | 964.29 | **+9.9%** | ✅ |
| 162 | dtsort-am | 1202.30 | 1093.95 | **+9.9%** | ✅ |
| 162 | dtsort-em | 1202.30 | 1088.08 | **+10.5%** | ✅ |
| 182 | dtsort-am | 1385.98 | 1303.03 | **+6.4%** | ✅ |
| 182 | dtsort-em | 1385.98 | 1305.72 | **+6.1%** | ✅ |
| 205 | dtsort-am | 1599.20 | 1546.81 | **+3.4%** | ✅ |
| 205 | dtsort-em | 1599.20 | 1547.17 | **+3.4%** | ✅ |
| 230 | dtsort-am | 1860.15 | 1820.32 | **+2.2%** | ✅ |
| 230 | dtsort-em | 1860.15 | 1804.40 | **+3.1%** | ✅ |
| 258 | dtsort-am | 2226.31 | 2165.15 | **+2.8%** | ✅ |
| 258 | dtsort-em | 2226.31 | 2150.36 | **+3.5%** | ✅ |
| 290 | dtsort-am | 2874.21 | 2490.90 | **+15.4%** | ✅ |
| 290 | dtsort-em | 2874.21 | 2488.82 | **+15.5%** | ✅ |
| 326 | dtsort-am | 3279.73 | 2874.60 | **+14.1%** | ✅ |
| 326 | dtsort-em | 3279.73 | 2926.00 | **+12.1%** | ✅ |
| 366 | dtsort-am | 3803.08 | 3358.35 | **+13.2%** | ✅ |
| 366 | dtsort-em | 3803.08 | 3399.58 | **+11.9%** | ✅ |
| 411 | dtsort-am | 4332.33 | 3873.14 | **+11.9%** | ✅ |
| 411 | dtsort-em | 4332.33 | 3842.27 | **+12.8%** | ✅ |
| 462 | dtsort-am | 4900.39 | 4478.28 | **+9.4%** | ✅ |
| 462 | dtsort-em | 4900.39 | 4461.16 | **+9.8%** | ✅ |
| 519 | dtsort-am | 5608.29 | 5229.95 | **+7.2%** | ✅ |
| 519 | dtsort-em | 5608.29 | 5169.57 | **+8.5%** | ✅ |
| 583 | dtsort-am | 6359.18 | 6023.78 | **+5.6%** | ✅ |
| 583 | dtsort-em | 6359.18 | 5914.92 | **+7.5%** | ✅ |
| 655 | dtsort-am | 7239.44 | 6962.83 | **+4.0%** | ✅ |
| 655 | dtsort-em | 7239.44 | 6810.63 | **+6.3%** | ✅ |
| 736 | dtsort-am | 8295.22 | 8120.84 | **+2.1%** | ✅ |
| 736 | dtsort-em | 8295.22 | 7951.08 | **+4.3%** | ✅ |
| 826 | dtsort-am | 9558.29 | 9432.54 | **+1.3%** | ✅ |
| 826 | dtsort-em | 9558.29 | 9198.91 | **+3.9%** | ✅ |
| 928 | dtsort-am | 10924.66 | 10765.24 | **+1.5%** | ✅ |
| 928 | dtsort-em | 10924.66 | 10610.98 | **+3.0%** | ✅ |
| 1,043 | dtsort-am | 12881.20 | 12544.67 | **+2.7%** | ✅ |
| 1,043 | dtsort-em | 12881.20 | 12379.90 | **+4.0%** | ✅ |
| 1,171 | dtsort-am | 15631.14 | 14374.45 | **+8.7%** | ✅ |
| 1,171 | dtsort-em | 15631.14 | 14361.21 | **+8.8%** | ✅ |
| 1,316 | dtsort-am | 17998.62 | 16436.56 | **+9.5%** | ✅ |
| 1,316 | dtsort-em | 17998.62 | 16363.11 | **+10.0%** | ✅ |
| 1,478 | dtsort-am | 20631.60 | 19063.17 | **+8.2%** | ✅ |
| 1,478 | dtsort-em | 20631.60 | 18871.90 | **+9.3%** | ✅ |
| 1,660 | dtsort-am | 23611.09 | 21906.71 | **+7.8%** | ✅ |
| 1,660 | dtsort-em | 23611.09 | 21688.10 | **+8.9%** | ✅ |
| 1,865 | dtsort-am | 26516.45 | 25229.43 | **+5.1%** | ✅ |
| 1,865 | dtsort-em | 26516.45 | 24764.35 | **+7.1%** | ✅ |
| 2,095 | dtsort-am | 29998.91 | 29255.58 | **+2.5%** | ✅ |
| 2,095 | dtsort-em | 29998.91 | 28540.11 | **+5.1%** | ✅ |
| 2,354 | dtsort-am | 34188.88 | 33606.69 | **+1.7%** | ✅ |
| 2,354 | dtsort-em | 34188.88 | 32829.61 | **+4.1%** | ✅ |
| 2,644 | dtsort-am | 39084.93 | 38296.10 | **+2.1%** | ✅ |
| 2,644 | dtsort-em | 39084.93 | 37398.20 | **+4.5%** | ✅ |
| 2,970 | dtsort-am | 44788.51 | 44052.01 | **+1.7%** | ✅ |
| 2,970 | dtsort-em | 44788.51 | 43242.81 | **+3.6%** | ✅ |
| 3,336 | dtsort-am | 50802.21 | 50578.01 | +0.4% | ✅ |
| 3,336 | dtsort-em | 50802.21 | 49867.09 | **+1.9%** | ✅ |
| 3,748 | dtsort-am | 57920.66 | 58157.33 | -0.4% | ✅ |
| 3,748 | dtsort-em | 57920.66 | 57562.65 | **+0.6%** | ✅ |
| 4,210 | dtsort-am | 68109.41 | 66716.41 | **+2.1%** | ✅ |
| 4,210 | dtsort-em | 68109.41 | 66036.20 | **+3.1%** | ✅ |
| 4,729 | dtsort-am | 81084.39 | 76180.18 | **+6.4%** | ✅ |
| 4,729 | dtsort-em | 81084.39 | 75849.70 | **+6.9%** | ✅ |
| 5,312 | dtsort-am | 92234.75 | 87010.06 | **+6.0%** | ✅ |
| 5,312 | dtsort-em | 92234.75 | 86783.89 | **+6.3%** | ✅ |
| 5,967 | dtsort-am | 105629.75 | 100098.05 | **+5.5%** | ✅ |
| 5,967 | dtsort-em | 105629.75 | 99221.23 | **+6.5%** | ✅ |
| 6,703 | dtsort-am | 120186.02 | 115362.02 | **+4.2%** | ✅ |
| 6,703 | dtsort-em | 120186.02 | 113381.75 | **+6.0%** | ✅ |
| 7,530 | dtsort-am | 136006.37 | 132381.34 | **+2.7%** | ✅ |
| 7,530 | dtsort-em | 136006.37 | 129743.67 | **+4.8%** | ✅ |
| 8,458 | dtsort-am | 154753.96 | 151919.78 | **+1.9%** | ✅ |
| 8,458 | dtsort-em | 154753.96 | 149487.57 | **+3.5%** | ✅ |
| 9,502 | dtsort-am | 176039.63 | 174226.40 | **+1.0%** | ✅ |
| 9,502 | dtsort-em | 176039.63 | 169915.96 | **+3.6%** | ✅ |
| 10,673 | dtsort-am | 199952.19 | 198426.19 | **+0.8%** | ✅ |
| 10,673 | dtsort-em | 199952.19 | 193429.32 | **+3.4%** | ✅ |
| 11,990 | dtsort-am | 226664.68 | 226848.42 | -0.1% | ✅ |
| 11,990 | dtsort-em | 226664.68 | 222148.30 | **+2.0%** | ✅ |
| 13,468 | dtsort-am | 257392.14 | 259785.57 | -0.9% | ✅ |
| 13,468 | dtsort-em | 257392.14 | 255856.35 | **+0.6%** | ✅ |
| 15,129 | dtsort-am | 293825.28 | 297598.78 | -1.3% | ⚠️ |
| 15,129 | dtsort-em | 293825.28 | 293522.35 | +0.1% | ✅ |
| 16,995 | dtsort-am | 344119.42 | 336258.46 | **+2.3%** | ✅ |
| 16,995 | dtsort-em | 344119.42 | 334831.45 | **+2.8%** | ✅ |
| 19,091 | dtsort-am | 402183.63 | 379896.48 | **+5.9%** | ✅ |
| 19,091 | dtsort-em | 402183.63 | 379548.08 | **+6.0%** | ✅ |
| 21,445 | dtsort-am | 456794.39 | 430654.17 | **+6.1%** | ✅ |
| 21,445 | dtsort-em | 456794.39 | 429791.48 | **+6.3%** | ✅ |
| 24,090 | dtsort-am | 527443.60 | 498471.40 | **+5.8%** | ✅ |
| 24,090 | dtsort-em | 527443.60 | 492796.83 | **+7.0%** | ✅ |
| 27,061 | dtsort-am | 593080.39 | 572484.81 | **+3.6%** | ✅ |
| 27,061 | dtsort-em | 593080.39 | 562993.18 | **+5.3%** | ✅ |
| 30,398 | dtsort-am | 671749.68 | 656703.86 | **+2.3%** | ✅ |
| 30,398 | dtsort-em | 671749.68 | 643963.99 | **+4.3%** | ✅ |
| 34,147 | dtsort-am | 758927.11 | 749262.40 | **+1.3%** | ✅ |
| 34,147 | dtsort-em | 758927.11 | 732643.55 | **+3.6%** | ✅ |
| 38,358 | dtsort-am | 856689.19 | 852887.42 | +0.4% | ✅ |
| 38,358 | dtsort-em | 856689.19 | 830444.17 | **+3.2%** | ✅ |
| 43,089 | dtsort-am | 974720.96 | 978941.42 | -0.4% | ✅ |
| 43,089 | dtsort-em | 974720.96 | 945927.43 | **+3.0%** | ✅ |
| 48,403 | dtsort-am | 1115207.24 | 1121376.29 | -0.6% | ✅ |
| 48,403 | dtsort-em | 1115207.24 | 1095837.87 | **+1.8%** | ✅ |
| 54,372 | dtsort-am | 1285762.42 | 1292453.32 | -0.5% | ✅ |
| 54,372 | dtsort-em | 1285762.42 | 1274519.71 | **+0.9%** | ✅ |
| 61,077 | dtsort-am | 1486133.55 | 1479991.63 | +0.4% | ✅ |
| 61,077 | dtsort-em | 1486133.55 | 1467532.52 | **+1.3%** | ✅ |
| 68,609 | dtsort-am | 1754369.32 | 1719578.54 | **+2.0%** | ✅ |
| 68,609 | dtsort-em | 1754369.32 | 1726492.13 | **+1.6%** | ✅ |
| 77,071 | dtsort-am | 2062464.29 | 1954029.61 | **+5.5%** | ✅ |
| 77,071 | dtsort-em | 2062464.29 | 1968835.13 | **+4.8%** | ✅ |
| 86,575 | dtsort-am | 2335635.00 | 2258891.07 | **+3.4%** | ✅ |
| 86,575 | dtsort-em | 2335635.00 | 2261301.78 | **+3.3%** | ✅ |
| 97,252 | dtsort-am | 2693529.83 | 2615880.21 | **+3.0%** | ✅ |
| 97,252 | dtsort-em | 2693529.83 | 2591483.21 | **+3.9%** | ✅ |
| 109,246 | dtsort-am | 3048599.49 | 3006102.17 | **+1.4%** | ✅ |
| 109,246 | dtsort-em | 3048599.49 | 2979658.59 | **+2.3%** | ✅ |
| 122,718 | dtsort-am | 3423145.21 | 3427945.56 | -0.1% | ✅ |
| 122,718 | dtsort-em | 3423145.21 | 3395505.72 | **+0.8%** | ✅ |
| 137,852 | dtsort-am | 3944376.42 | 3966006.47 | -0.5% | ✅ |
| 137,852 | dtsort-em | 3944376.42 | 3926273.74 | +0.5% | ✅ |
| 154,853 | dtsort-am | 4496008.33 | 4523321.75 | -0.6% | ✅ |
| 154,853 | dtsort-em | 4496008.33 | 4459086.87 | **+0.8%** | ✅ |
| 173,950 | dtsort-am | 5139609.65 | 5152022.08 | -0.2% | ✅ |
| 173,950 | dtsort-em | 5139609.65 | 5071522.53 | **+1.3%** | ✅ |
| 195,402 | dtsort-am | 5825780.72 | 5892502.84 | -1.1% | ⚠️ |
| 195,402 | dtsort-em | 5825780.72 | 5844636.75 | -0.3% | ✅ |
| 219,500 | dtsort-am | 6727613.68 | 6718921.92 | +0.1% | ✅ |
| 219,500 | dtsort-em | 6727613.68 | 6690343.04 | **+0.6%** | ✅ |
| 246,569 | dtsort-am | 7559769.23 | 7627515.56 | -0.9% | ✅ |
| 246,569 | dtsort-em | 7559769.23 | 7632981.68 | -1.0% | ✅ |
| 276,977 | dtsort-am | 8761694.62 | 8765237.34 | -0.0% | ✅ |
| 276,977 | dtsort-em | 8761694.62 | 8752908.23 | +0.1% | ✅ |
| 311,135 | dtsort-am | 10114117.16 | 9956080.96 | **+1.6%** | ✅ |
| 311,135 | dtsort-em | 10114117.16 | 9935022.63 | **+1.8%** | ✅ |
| 349,506 | dtsort-am | 11481555.33 | 11303190.19 | **+1.6%** | ✅ |
| 349,506 | dtsort-em | 11481555.33 | 11275838.04 | **+1.8%** | ✅ |
| 392,608 | dtsort-am | 13049400.94 | 12902395.05 | **+1.1%** | ✅ |
| 392,608 | dtsort-em | 13049400.94 | 12863863.43 | **+1.4%** | ✅ |
| 441,026 | dtsort-am | 14656295.21 | 14516229.17 | **+1.0%** | ✅ |
| 441,026 | dtsort-em | 14656295.21 | 14453033.00 | **+1.4%** | ✅ |
| 495,415 | dtsort-am | 16653991.07 | 16532247.03 | **+0.7%** | ✅ |
| 495,415 | dtsort-em | 16653991.07 | 16433535.86 | **+1.3%** | ✅ |
| 556,512 | dtsort-am | 18714748.87 | 18697898.65 | +0.1% | ✅ |
| 556,512 | dtsort-em | 18714748.87 | 18606572.36 | **+0.6%** | ✅ |
| 625,143 | dtsort-am | 21235098.48 | 21037045.45 | **+0.9%** | ✅ |
| 625,143 | dtsort-em | 21235098.48 | 20897051.76 | **+1.6%** | ✅ |
| 702,238 | dtsort-am | 24114372.14 | 23889843.37 | **+0.9%** | ✅ |
| 702,238 | dtsort-em | 24114372.14 | 23715216.97 | **+1.7%** | ✅ |
| 788,841 | dtsort-am | 27129307.69 | 27254386.20 | -0.5% | ✅ |
| 788,841 | dtsort-em | 27129307.69 | 27103982.35 | +0.1% | ✅ |
| 886,124 | dtsort-am | 30805561.61 | 30951197.48 | -0.5% | ✅ |
| 886,124 | dtsort-em | 30805561.61 | 30787815.22 | +0.1% | ✅ |
| 995,405 | dtsort-am | 34953808.34 | 35044091.71 | -0.3% | ✅ |
| 995,405 | dtsort-em | 34953808.34 | 34890729.15 | +0.2% | ✅ |
| 1,118,162 | dtsort-am | 39824571.72 | 39797650.44 | +0.1% | ✅ |
| 1,118,162 | dtsort-em | 39824571.72 | 39775356.50 | +0.1% | ✅ |
| 1,256,058 | dtsort-am | 45872502.81 | 44932263.00 | **+2.1%** | ✅ |
| 1,256,058 | dtsort-em | 45872502.81 | 44988346.30 | **+2.0%** | ✅ |
| 1,410,960 | dtsort-am | 51793073.77 | 50895348.21 | **+1.8%** | ✅ |
| 1,410,960 | dtsort-em | 51793073.77 | 50944583.36 | **+1.7%** | ✅ |
| 1,584,966 | dtsort-am | 58667083.35 | 57811468.74 | **+1.5%** | ✅ |
| 1,584,966 | dtsort-em | 58667083.35 | 57759093.74 | **+1.6%** | ✅ |
| 1,780,430 | dtsort-am | 66266587.49 | 65446856.09 | **+1.3%** | ✅ |
| 1,780,430 | dtsort-em | 66266587.49 | 65193545.45 | **+1.6%** | ✅ |
| 2,000,000 | dtsort-am | 74851782.42 | 73994504.55 | **+1.2%** | ✅ |
| 2,000,000 | dtsort-em | 74851782.42 | 73501555.56 | **+1.8%** | ✅ |

#### dt (IntNaked (Reverse))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort-am | 12.67 | 4.42 | **+186.9%** | ✅ |
| 2 | dtsort-em | 12.67 | 3.22 | **+293.9%** | ✅ |
| 3 | dtsort-am | 18.21 | 5.68 | **+220.6%** | ✅ |
| 3 | dtsort-em | 18.21 | 4.09 | **+345.1%** | ✅ |
| 4 | dtsort-am | 19.03 | 6.99 | **+172.2%** | ✅ |
| 4 | dtsort-em | 19.03 | 4.70 | **+304.7%** | ✅ |
| 5 | dtsort-am | 21.91 | 6.92 | **+216.8%** | ✅ |
| 5 | dtsort-em | 21.91 | 4.97 | **+340.3%** | ✅ |
| 6 | dtsort-am | 23.61 | 7.49 | **+215.1%** | ✅ |
| 6 | dtsort-em | 23.61 | 6.05 | **+290.4%** | ✅ |
| 7 | dtsort-am | 24.12 | 8.81 | **+173.9%** | ✅ |
| 7 | dtsort-em | 24.12 | 7.53 | **+220.2%** | ✅ |
| 8 | dtsort-am | 26.60 | 12.29 | **+116.4%** | ✅ |
| 8 | dtsort-em | 26.60 | 9.85 | **+170.0%** | ✅ |
| 9 | dtsort-am | 27.99 | 11.72 | **+138.7%** | ✅ |
| 9 | dtsort-em | 27.99 | 12.80 | **+118.6%** | ✅ |
| 10 | dtsort-am | 28.66 | 11.52 | **+148.7%** | ✅ |
| 10 | dtsort-em | 28.66 | 11.72 | **+144.6%** | ✅ |
| 20 | dtsort-am | 68.88 | 45.98 | **+49.8%** | ✅ |
| 20 | dtsort-em | 68.88 | 47.04 | **+46.4%** | ✅ |
| 22 | dtsort-am | 76.89 | 55.19 | **+39.3%** | ✅ |
| 22 | dtsort-em | 76.89 | 54.28 | **+41.7%** | ✅ |
| 25 | dtsort-am | 85.59 | 67.71 | **+26.4%** | ✅ |
| 25 | dtsort-em | 85.59 | 69.91 | **+22.4%** | ✅ |
| 28 | dtsort-am | 92.66 | 78.21 | **+18.5%** | ✅ |
| 28 | dtsort-em | 92.66 | 83.57 | **+10.9%** | ✅ |
| 32 | dtsort-am | 98.47 | 101.80 | -3.3% | ⚠️ |
| 32 | dtsort-em | 98.47 | 101.28 | -2.8% | ⚠️ |
| 36 | dtsort-am | 126.36 | 120.45 | **+4.9%** | ✅ |
| 36 | dtsort-em | 126.36 | 125.11 | **+1.0%** | ✅ |
| 40 | dtsort-am | 141.40 | 100.40 | **+40.8%** | ✅ |
| 40 | dtsort-em | 141.40 | 104.74 | **+35.0%** | ✅ |
| 45 | dtsort-am | 167.19 | 141.52 | **+18.1%** | ✅ |
| 45 | dtsort-em | 167.19 | 146.16 | **+14.4%** | ✅ |
| 51 | dtsort-am | 196.44 | 169.41 | **+16.0%** | ✅ |
| 51 | dtsort-em | 196.44 | 176.00 | **+11.6%** | ✅ |
| 57 | dtsort-am | 227.60 | 218.54 | **+4.1%** | ✅ |
| 57 | dtsort-em | 227.60 | 219.41 | **+3.7%** | ✅ |
| 64 | dtsort-am | 254.81 | 235.33 | **+8.3%** | ✅ |
| 64 | dtsort-em | 254.81 | 238.57 | **+6.8%** | ✅ |
| 72 | dtsort-am | 345.81 | 269.59 | **+28.3%** | ✅ |
| 72 | dtsort-em | 345.81 | 276.72 | **+25.0%** | ✅ |
| 81 | dtsort-am | 379.41 | 291.13 | **+30.3%** | ✅ |
| 81 | dtsort-em | 379.41 | 291.40 | **+30.2%** | ✅ |
| 91 | dtsort-am | 443.00 | 372.73 | **+18.9%** | ✅ |
| 91 | dtsort-em | 443.00 | 369.70 | **+19.8%** | ✅ |
| 102 | dtsort-am | 507.59 | 433.42 | **+17.1%** | ✅ |
| 102 | dtsort-em | 507.59 | 436.01 | **+16.4%** | ✅ |
| 114 | dtsort-am | 558.37 | 499.92 | **+11.7%** | ✅ |
| 114 | dtsort-em | 558.37 | 507.05 | **+10.1%** | ✅ |
| 129 | dtsort-am | 613.71 | 581.15 | **+5.6%** | ✅ |
| 129 | dtsort-em | 613.71 | 584.95 | **+4.9%** | ✅ |
| 144 | dtsort-am | 696.34 | 622.06 | **+11.9%** | ✅ |
| 144 | dtsort-em | 696.34 | 626.73 | **+11.1%** | ✅ |
| 162 | dtsort-am | 802.33 | 674.61 | **+18.9%** | ✅ |
| 162 | dtsort-em | 802.33 | 678.57 | **+18.2%** | ✅ |
| 182 | dtsort-am | 932.87 | 832.44 | **+12.1%** | ✅ |
| 182 | dtsort-em | 932.87 | 843.95 | **+10.5%** | ✅ |
| 205 | dtsort-am | 1099.42 | 997.62 | **+10.2%** | ✅ |
| 205 | dtsort-em | 1099.42 | 1008.30 | **+9.0%** | ✅ |
| 230 | dtsort-am | 1261.95 | 1149.40 | **+9.8%** | ✅ |
| 230 | dtsort-em | 1261.95 | 1160.19 | **+8.8%** | ✅ |
| 258 | dtsort-am | 1455.37 | 1358.81 | **+7.1%** | ✅ |
| 258 | dtsort-em | 1455.37 | 1366.56 | **+6.5%** | ✅ |
| 290 | dtsort-am | 1825.30 | 1504.10 | **+21.4%** | ✅ |
| 290 | dtsort-em | 1825.30 | 1513.26 | **+20.6%** | ✅ |
| 326 | dtsort-am | 2119.03 | 1699.14 | **+24.7%** | ✅ |
| 326 | dtsort-em | 2119.03 | 1728.91 | **+22.6%** | ✅ |
| 366 | dtsort-am | 2404.69 | 2032.16 | **+18.3%** | ✅ |
| 366 | dtsort-em | 2404.69 | 2093.15 | **+14.9%** | ✅ |
| 411 | dtsort-am | 2727.65 | 2389.55 | **+14.1%** | ✅ |
| 411 | dtsort-em | 2727.65 | 2393.90 | **+13.9%** | ✅ |
| 462 | dtsort-am | 3064.21 | 2727.48 | **+12.3%** | ✅ |
| 462 | dtsort-em | 3064.21 | 2725.28 | **+12.4%** | ✅ |
| 519 | dtsort-am | 3386.16 | 3135.42 | **+8.0%** | ✅ |
| 519 | dtsort-em | 3386.16 | 3144.09 | **+7.7%** | ✅ |
| 583 | dtsort-am | 3874.29 | 3483.65 | **+11.2%** | ✅ |
| 583 | dtsort-em | 3874.29 | 3473.24 | **+11.5%** | ✅ |
| 655 | dtsort-am | 4402.00 | 3850.37 | **+14.3%** | ✅ |
| 655 | dtsort-em | 4402.00 | 3839.81 | **+14.6%** | ✅ |
| 736 | dtsort-am | 5123.71 | 4757.24 | **+7.7%** | ✅ |
| 736 | dtsort-em | 5123.71 | 4741.90 | **+8.1%** | ✅ |
| 826 | dtsort-am | 5848.92 | 5551.26 | **+5.4%** | ✅ |
| 826 | dtsort-em | 5848.92 | 5500.85 | **+6.3%** | ✅ |
| 928 | dtsort-am | 6721.01 | 6359.48 | **+5.7%** | ✅ |
| 928 | dtsort-em | 6721.01 | 6317.56 | **+6.4%** | ✅ |
| 1,043 | dtsort-am | 7767.43 | 7161.45 | **+8.5%** | ✅ |
| 1,043 | dtsort-em | 7767.43 | 7173.37 | **+8.3%** | ✅ |
| 1,171 | dtsort-am | 9454.13 | 8087.31 | **+16.9%** | ✅ |
| 1,171 | dtsort-em | 9454.13 | 8124.78 | **+16.4%** | ✅ |
| 1,316 | dtsort-am | 10797.99 | 9093.82 | **+18.7%** | ✅ |
| 1,316 | dtsort-em | 10797.99 | 9109.13 | **+18.5%** | ✅ |
| 1,478 | dtsort-am | 12241.62 | 10857.57 | **+12.7%** | ✅ |
| 1,478 | dtsort-em | 12241.62 | 10875.63 | **+12.6%** | ✅ |
| 1,660 | dtsort-am | 13891.37 | 12496.02 | **+11.2%** | ✅ |
| 1,660 | dtsort-em | 13891.37 | 12479.40 | **+11.3%** | ✅ |
| 1,865 | dtsort-am | 15532.40 | 14333.32 | **+8.4%** | ✅ |
| 1,865 | dtsort-em | 15532.40 | 14248.81 | **+9.0%** | ✅ |
| 2,095 | dtsort-am | 17323.40 | 16187.54 | **+7.0%** | ✅ |
| 2,095 | dtsort-em | 17323.40 | 16047.23 | **+8.0%** | ✅ |
| 2,354 | dtsort-am | 19563.37 | 18045.04 | **+8.4%** | ✅ |
| 2,354 | dtsort-em | 19563.37 | 17891.45 | **+9.3%** | ✅ |
| 2,644 | dtsort-am | 22215.40 | 20289.73 | **+9.5%** | ✅ |
| 2,644 | dtsort-em | 22215.40 | 20031.93 | **+10.9%** | ✅ |
| 2,970 | dtsort-am | 25650.99 | 24041.21 | **+6.7%** | ✅ |
| 2,970 | dtsort-em | 25650.99 | 23834.71 | **+7.6%** | ✅ |
| 3,336 | dtsort-am | 29056.09 | 27751.31 | **+4.7%** | ✅ |
| 3,336 | dtsort-em | 29056.09 | 27678.27 | **+5.0%** | ✅ |
| 3,748 | dtsort-am | 33234.95 | 31587.75 | **+5.2%** | ✅ |
| 3,748 | dtsort-em | 33234.95 | 31483.39 | **+5.6%** | ✅ |
| 4,210 | dtsort-am | 38692.31 | 35846.43 | **+7.9%** | ✅ |
| 4,210 | dtsort-em | 38692.31 | 35824.17 | **+8.0%** | ✅ |
| 4,729 | dtsort-am | 46259.39 | 40415.77 | **+14.5%** | ✅ |
| 4,729 | dtsort-em | 46259.39 | 40426.48 | **+14.4%** | ✅ |
| 5,312 | dtsort-am | 52578.88 | 45355.81 | **+15.9%** | ✅ |
| 5,312 | dtsort-em | 52578.88 | 45434.83 | **+15.7%** | ✅ |
| 5,967 | dtsort-am | 59332.04 | 53877.29 | **+10.1%** | ✅ |
| 5,967 | dtsort-em | 59332.04 | 53998.22 | **+9.9%** | ✅ |
| 6,703 | dtsort-am | 67771.87 | 61600.41 | **+10.0%** | ✅ |
| 6,703 | dtsort-em | 67771.87 | 61422.39 | **+10.3%** | ✅ |
| 7,530 | dtsort-am | 75735.83 | 69591.22 | **+8.8%** | ✅ |
| 7,530 | dtsort-em | 75735.83 | 69138.90 | **+9.5%** | ✅ |
| 8,458 | dtsort-am | 84218.07 | 80091.53 | **+5.2%** | ✅ |
| 8,458 | dtsort-em | 84218.07 | 79488.65 | **+5.9%** | ✅ |
| 9,502 | dtsort-am | 95061.77 | 87752.17 | **+8.3%** | ✅ |
| 9,502 | dtsort-em | 95061.77 | 86966.44 | **+9.3%** | ✅ |
| 10,673 | dtsort-am | 107741.89 | 99917.17 | **+7.8%** | ✅ |
| 10,673 | dtsort-em | 107741.89 | 98919.65 | **+8.9%** | ✅ |
| 11,990 | dtsort-am | 123044.21 | 116230.84 | **+5.9%** | ✅ |
| 11,990 | dtsort-em | 123044.21 | 115539.35 | **+6.5%** | ✅ |
| 13,468 | dtsort-am | 138774.04 | 132839.62 | **+4.5%** | ✅ |
| 13,468 | dtsort-em | 138774.04 | 131891.52 | **+5.2%** | ✅ |
| 15,129 | dtsort-am | 158342.38 | 149339.82 | **+6.0%** | ✅ |
| 15,129 | dtsort-em | 158342.38 | 148797.50 | **+6.4%** | ✅ |
| 16,995 | dtsort-am | 184653.99 | 169803.56 | **+8.7%** | ✅ |
| 16,995 | dtsort-em | 184653.99 | 169627.22 | **+8.9%** | ✅ |
| 19,091 | dtsort-am | 220309.12 | 189445.23 | **+16.3%** | ✅ |
| 19,091 | dtsort-em | 220309.12 | 189593.72 | **+16.2%** | ✅ |
| 21,445 | dtsort-am | 248114.44 | 213832.87 | **+16.0%** | ✅ |
| 21,445 | dtsort-em | 248114.44 | 214333.24 | **+15.8%** | ✅ |
| 24,090 | dtsort-am | 280017.49 | 248631.50 | **+12.6%** | ✅ |
| 24,090 | dtsort-em | 280017.49 | 249248.66 | **+12.3%** | ✅ |
| 27,061 | dtsort-am | 314831.76 | 283254.28 | **+11.1%** | ✅ |
| 27,061 | dtsort-em | 314831.76 | 281791.82 | **+11.7%** | ✅ |
| 30,398 | dtsort-am | 351123.72 | 317389.74 | **+10.6%** | ✅ |
| 30,398 | dtsort-em | 351123.72 | 314906.11 | **+11.5%** | ✅ |
| 34,147 | dtsort-am | 393037.27 | 364028.53 | **+8.0%** | ✅ |
| 34,147 | dtsort-em | 393037.27 | 360765.58 | **+8.9%** | ✅ |
| 38,358 | dtsort-am | 437011.21 | 397675.38 | **+9.9%** | ✅ |
| 38,358 | dtsort-em | 437011.21 | 394039.56 | **+10.9%** | ✅ |
| 43,089 | dtsort-am | 499462.80 | 452986.71 | **+10.3%** | ✅ |
| 43,089 | dtsort-em | 499462.80 | 447941.12 | **+11.5%** | ✅ |
| 48,403 | dtsort-am | 561703.81 | 527452.81 | **+6.5%** | ✅ |
| 48,403 | dtsort-em | 561703.81 | 523661.87 | **+7.3%** | ✅ |
| 54,372 | dtsort-am | 641859.41 | 603589.16 | **+6.3%** | ✅ |
| 54,372 | dtsort-em | 641859.41 | 599130.37 | **+7.1%** | ✅ |
| 61,077 | dtsort-am | 746113.69 | 677673.68 | **+10.1%** | ✅ |
| 61,077 | dtsort-em | 746113.69 | 674593.35 | **+10.6%** | ✅ |
| 68,609 | dtsort-am | 891518.59 | 778695.52 | **+14.5%** | ✅ |
| 68,609 | dtsort-em | 891518.59 | 769919.67 | **+15.8%** | ✅ |
| 77,071 | dtsort-am | 1071781.00 | 870534.42 | **+23.1%** | ✅ |
| 77,071 | dtsort-em | 1071781.00 | 862552.29 | **+24.3%** | ✅ |
| 86,575 | dtsort-am | 1219061.81 | 979716.61 | **+24.4%** | ✅ |
| 86,575 | dtsort-em | 1219061.81 | 975471.63 | **+25.0%** | ✅ |
| 97,252 | dtsort-am | 1393497.29 | 1144131.77 | **+21.8%** | ✅ |
| 97,252 | dtsort-em | 1393497.29 | 1125463.30 | **+23.8%** | ✅ |
| 109,246 | dtsort-am | 1531015.68 | 1309386.07 | **+16.9%** | ✅ |
| 109,246 | dtsort-em | 1531015.68 | 1282840.15 | **+19.3%** | ✅ |
| 122,718 | dtsort-am | 1728481.30 | 1482960.65 | **+16.6%** | ✅ |
| 122,718 | dtsort-em | 1728481.30 | 1444844.86 | **+19.6%** | ✅ |
| 137,852 | dtsort-am | 1945534.14 | 1724997.10 | **+12.8%** | ✅ |
| 137,852 | dtsort-em | 1945534.14 | 1679760.65 | **+15.8%** | ✅ |
| 154,853 | dtsort-am | 2169126.44 | 1914507.53 | **+13.3%** | ✅ |
| 154,853 | dtsort-em | 2169126.44 | 1852549.14 | **+17.1%** | ✅ |
| 173,950 | dtsort-am | 2479903.97 | 2170198.51 | **+14.3%** | ✅ |
| 173,950 | dtsort-em | 2479903.97 | 2113110.90 | **+17.4%** | ✅ |
| 195,402 | dtsort-am | 2811770.07 | 2503625.61 | **+12.3%** | ✅ |
| 195,402 | dtsort-em | 2811770.07 | 2457662.38 | **+14.4%** | ✅ |
| 219,500 | dtsort-am | 3241385.61 | 2864258.12 | **+13.2%** | ✅ |
| 219,500 | dtsort-em | 3241385.61 | 2824851.51 | **+14.7%** | ✅ |
| 246,569 | dtsort-am | 3617613.76 | 3206556.45 | **+12.8%** | ✅ |
| 246,569 | dtsort-em | 3617613.76 | 3178116.75 | **+13.8%** | ✅ |
| 276,977 | dtsort-am | 4333280.86 | 3730590.73 | **+16.2%** | ✅ |
| 276,977 | dtsort-em | 4333280.86 | 3722624.11 | **+16.4%** | ✅ |
| 311,135 | dtsort-am | 4969954.10 | 4184240.15 | **+18.8%** | ✅ |
| 311,135 | dtsort-em | 4969954.10 | 4176436.38 | **+19.0%** | ✅ |
| 349,506 | dtsort-am | 5716310.81 | 4743409.86 | **+20.5%** | ✅ |
| 349,506 | dtsort-em | 5716310.81 | 4723357.88 | **+21.0%** | ✅ |
| 392,608 | dtsort-am | 6324564.36 | 5481334.00 | **+15.4%** | ✅ |
| 392,608 | dtsort-em | 6324564.36 | 5463788.03 | **+15.8%** | ✅ |
| 441,026 | dtsort-am | 7067045.97 | 6205291.66 | **+13.9%** | ✅ |
| 441,026 | dtsort-em | 7067045.97 | 6157737.99 | **+14.8%** | ✅ |
| 495,415 | dtsort-am | 7915231.06 | 6974247.06 | **+13.5%** | ✅ |
| 495,415 | dtsort-em | 7915231.06 | 6916405.83 | **+14.4%** | ✅ |
| 556,512 | dtsort-am | 8880371.36 | 7974009.10 | **+11.4%** | ✅ |
| 556,512 | dtsort-em | 8880371.36 | 7880936.08 | **+12.7%** | ✅ |
| 625,143 | dtsort-am | 10008522.94 | 8813208.86 | **+13.6%** | ✅ |
| 625,143 | dtsort-em | 10008522.94 | 8738505.74 | **+14.5%** | ✅ |
| 702,238 | dtsort-am | 11453172.13 | 9989849.64 | **+14.6%** | ✅ |
| 702,238 | dtsort-em | 11453172.13 | 9923655.36 | **+15.4%** | ✅ |
| 788,841 | dtsort-am | 12732381.06 | 11554827.87 | **+10.2%** | ✅ |
| 788,841 | dtsort-em | 12732381.06 | 11514629.17 | **+10.6%** | ✅ |
| 886,124 | dtsort-am | 14716038.13 | 13054312.11 | **+12.7%** | ✅ |
| 886,124 | dtsort-em | 14716038.13 | 13055625.79 | **+12.7%** | ✅ |
| 995,405 | dtsort-am | 16531103.17 | 14753095.74 | **+12.1%** | ✅ |
| 995,405 | dtsort-em | 16531103.17 | 14725154.50 | **+12.3%** | ✅ |
| 1,118,162 | dtsort-am | 18971600.21 | 16902784.54 | **+12.2%** | ✅ |
| 1,118,162 | dtsort-em | 18971600.21 | 16894102.19 | **+12.3%** | ✅ |
| 1,256,058 | dtsort-am | 22088690.09 | 18738916.65 | **+17.9%** | ✅ |
| 1,256,058 | dtsort-em | 22088690.09 | 18872298.43 | **+17.0%** | ✅ |
| 1,410,960 | dtsort-am | 24936294.64 | 21330714.63 | **+16.9%** | ✅ |
| 1,410,960 | dtsort-em | 24936294.64 | 21356468.43 | **+16.8%** | ✅ |
| 1,584,966 | dtsort-am | 27551458.37 | 24382653.76 | **+13.0%** | ✅ |
| 1,584,966 | dtsort-em | 27551458.37 | 24388528.25 | **+13.0%** | ✅ |
| 1,780,430 | dtsort-am | 30937537.91 | 27812851.69 | **+11.2%** | ✅ |
| 1,780,430 | dtsort-em | 30937537.91 | 27793921.68 | **+11.3%** | ✅ |
| 2,000,000 | dtsort-am | 34826400.00 | 30978766.30 | **+12.4%** | ✅ |
| 2,000,000 | dtsort-em | 34826400.00 | 30784159.39 | **+13.1%** | ✅ |

#### dt (IntNaked (Sorted))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort-am | 12.71 | 4.39 | **+189.7%** | ✅ |
| 2 | dtsort-em | 12.71 | 3.20 | **+296.7%** | ✅ |
| 3 | dtsort-am | 17.71 | 5.09 | **+248.3%** | ✅ |
| 3 | dtsort-em | 17.71 | 3.69 | **+379.5%** | ✅ |
| 4 | dtsort-am | 18.49 | 6.08 | **+203.9%** | ✅ |
| 4 | dtsort-em | 18.49 | 4.64 | **+298.5%** | ✅ |
| 5 | dtsort-am | 20.51 | 6.47 | **+216.7%** | ✅ |
| 5 | dtsort-em | 20.51 | 5.33 | **+284.5%** | ✅ |
| 6 | dtsort-am | 21.58 | 6.28 | **+243.5%** | ✅ |
| 6 | dtsort-em | 21.58 | 5.82 | **+270.5%** | ✅ |
| 7 | dtsort-am | 21.72 | 7.21 | **+201.1%** | ✅ |
| 7 | dtsort-em | 21.72 | 7.04 | **+208.7%** | ✅ |
| 8 | dtsort-am | 22.95 | 8.77 | **+161.8%** | ✅ |
| 8 | dtsort-em | 22.95 | 8.58 | **+167.4%** | ✅ |
| 9 | dtsort-am | 23.73 | 9.83 | **+141.5%** | ✅ |
| 9 | dtsort-em | 23.73 | 11.16 | **+112.7%** | ✅ |
| 10 | dtsort-am | 24.67 | 9.98 | **+147.2%** | ✅ |
| 10 | dtsort-em | 24.67 | 9.90 | **+149.3%** | ✅ |
| 20 | dtsort-am | 60.52 | 32.29 | **+87.4%** | ✅ |
| 20 | dtsort-em | 60.52 | 42.55 | **+42.2%** | ✅ |
| 22 | dtsort-am | 65.13 | 38.65 | **+68.5%** | ✅ |
| 22 | dtsort-em | 65.13 | 52.93 | **+23.0%** | ✅ |
| 25 | dtsort-am | 72.59 | 40.87 | **+77.6%** | ✅ |
| 25 | dtsort-em | 72.59 | 59.01 | **+23.0%** | ✅ |
| 28 | dtsort-am | 81.39 | 48.10 | **+69.2%** | ✅ |
| 28 | dtsort-em | 81.39 | 62.03 | **+31.2%** | ✅ |
| 32 | dtsort-am | 85.65 | 50.94 | **+68.1%** | ✅ |
| 32 | dtsort-em | 85.65 | 77.75 | **+10.2%** | ✅ |
| 36 | dtsort-am | 105.11 | 57.53 | **+82.7%** | ✅ |
| 36 | dtsort-em | 105.11 | 88.25 | **+19.1%** | ✅ |
| 40 | dtsort-am | 114.28 | 57.93 | **+97.3%** | ✅ |
| 40 | dtsort-em | 114.28 | 93.72 | **+21.9%** | ✅ |
| 45 | dtsort-am | 125.37 | 73.28 | **+71.1%** | ✅ |
| 45 | dtsort-em | 125.37 | 119.21 | **+5.2%** | ✅ |
| 51 | dtsort-am | 139.47 | 77.13 | **+80.8%** | ✅ |
| 51 | dtsort-em | 139.47 | 135.51 | **+2.9%** | ✅ |
| 57 | dtsort-am | 157.63 | 84.21 | **+87.2%** | ✅ |
| 57 | dtsort-em | 157.63 | 145.28 | **+8.5%** | ✅ |
| 64 | dtsort-am | 172.12 | 94.92 | **+81.3%** | ✅ |
| 64 | dtsort-em | 172.12 | 180.15 | -4.5% | ⚠️ |
| 72 | dtsort-am | 312.14 | 106.88 | **+192.0%** | ✅ |
| 72 | dtsort-em | 312.14 | 215.21 | **+45.0%** | ✅ |
| 81 | dtsort-am | 342.41 | 118.16 | **+189.8%** | ✅ |
| 81 | dtsort-em | 342.41 | 274.66 | **+24.7%** | ✅ |
| 91 | dtsort-am | 394.64 | 135.22 | **+191.9%** | ✅ |
| 91 | dtsort-em | 394.64 | 303.63 | **+30.0%** | ✅ |
| 102 | dtsort-am | 435.01 | 149.02 | **+191.9%** | ✅ |
| 102 | dtsort-em | 435.01 | 342.80 | **+26.9%** | ✅ |
| 114 | dtsort-am | 484.25 | 169.59 | **+185.5%** | ✅ |
| 114 | dtsort-em | 484.25 | 375.48 | **+29.0%** | ✅ |
| 129 | dtsort-am | 536.66 | 180.84 | **+196.8%** | ✅ |
| 129 | dtsort-em | 536.66 | 435.49 | **+23.2%** | ✅ |
| 144 | dtsort-am | 593.58 | 200.26 | **+196.4%** | ✅ |
| 144 | dtsort-em | 593.58 | 501.26 | **+18.4%** | ✅ |
| 162 | dtsort-am | 682.33 | 216.64 | **+215.0%** | ✅ |
| 162 | dtsort-em | 682.33 | 599.73 | **+13.8%** | ✅ |
| 182 | dtsort-am | 756.70 | 246.58 | **+206.9%** | ✅ |
| 182 | dtsort-em | 756.70 | 672.20 | **+12.6%** | ✅ |
| 205 | dtsort-am | 875.84 | 285.99 | **+206.3%** | ✅ |
| 205 | dtsort-em | 875.84 | 793.36 | **+10.4%** | ✅ |
| 230 | dtsort-am | 998.79 | 313.46 | **+218.6%** | ✅ |
| 230 | dtsort-em | 998.79 | 957.07 | **+4.4%** | ✅ |
| 258 | dtsort-am | 1113.37 | 354.70 | **+213.9%** | ✅ |
| 258 | dtsort-em | 1113.37 | 1088.80 | **+2.3%** | ✅ |
| 290 | dtsort-am | 1690.76 | 406.60 | **+315.8%** | ✅ |
| 290 | dtsort-em | 1690.76 | 1320.49 | **+28.0%** | ✅ |
| 326 | dtsort-am | 1914.56 | 453.15 | **+322.5%** | ✅ |
| 326 | dtsort-em | 1914.56 | 1590.12 | **+20.4%** | ✅ |
| 366 | dtsort-am | 2174.96 | 516.14 | **+321.4%** | ✅ |
| 366 | dtsort-em | 2174.96 | 1740.93 | **+24.9%** | ✅ |
| 411 | dtsort-am | 2453.11 | 586.40 | **+318.3%** | ✅ |
| 411 | dtsort-em | 2453.11 | 1978.05 | **+24.0%** | ✅ |
| 462 | dtsort-am | 2757.11 | 660.39 | **+317.5%** | ✅ |
| 462 | dtsort-em | 2757.11 | 2299.19 | **+19.9%** | ✅ |
| 519 | dtsort-am | 3047.62 | 734.80 | **+314.8%** | ✅ |
| 519 | dtsort-em | 3047.62 | 2579.11 | **+18.2%** | ✅ |
| 583 | dtsort-am | 3402.20 | 831.34 | **+309.2%** | ✅ |
| 583 | dtsort-em | 3402.20 | 3011.07 | **+13.0%** | ✅ |
| 655 | dtsort-am | 3869.14 | 894.91 | **+332.4%** | ✅ |
| 655 | dtsort-em | 3869.14 | 3462.18 | **+11.8%** | ✅ |
| 736 | dtsort-am | 4322.31 | 1045.14 | **+313.6%** | ✅ |
| 736 | dtsort-em | 4322.31 | 3951.98 | **+9.4%** | ✅ |
| 826 | dtsort-am | 5005.44 | 1205.16 | **+315.3%** | ✅ |
| 826 | dtsort-em | 5005.44 | 4596.82 | **+8.9%** | ✅ |
| 928 | dtsort-am | 5469.44 | 1365.98 | **+300.4%** | ✅ |
| 928 | dtsort-em | 5469.44 | 5452.65 | +0.3% | ✅ |
| 1,043 | dtsort-am | 6479.85 | 1514.54 | **+327.8%** | ✅ |
| 1,043 | dtsort-em | 6479.85 | 6358.93 | **+1.9%** | ✅ |
| 1,171 | dtsort-am | 8884.17 | 1761.84 | **+404.3%** | ✅ |
| 1,171 | dtsort-em | 8884.17 | 7385.74 | **+20.3%** | ✅ |
| 1,316 | dtsort-am | 9896.59 | 1963.67 | **+404.0%** | ✅ |
| 1,316 | dtsort-em | 9896.59 | 8527.16 | **+16.1%** | ✅ |
| 1,478 | dtsort-am | 11121.08 | 2226.72 | **+399.4%** | ✅ |
| 1,478 | dtsort-em | 11121.08 | 9515.63 | **+16.9%** | ✅ |
| 1,660 | dtsort-am | 12540.18 | 2519.53 | **+397.7%** | ✅ |
| 1,660 | dtsort-em | 12540.18 | 10901.77 | **+15.0%** | ✅ |
| 1,865 | dtsort-am | 14072.10 | 2834.48 | **+396.5%** | ✅ |
| 1,865 | dtsort-em | 14072.10 | 12540.32 | **+12.2%** | ✅ |
| 2,095 | dtsort-am | 15955.39 | 3178.80 | **+401.9%** | ✅ |
| 2,095 | dtsort-em | 15955.39 | 14114.13 | **+13.0%** | ✅ |
| 2,354 | dtsort-am | 17825.36 | 3620.10 | **+392.4%** | ✅ |
| 2,354 | dtsort-em | 17825.36 | 16267.96 | **+9.6%** | ✅ |
| 2,644 | dtsort-am | 20041.25 | 3933.71 | **+409.5%** | ✅ |
| 2,644 | dtsort-em | 20041.25 | 18667.84 | **+7.4%** | ✅ |
| 2,970 | dtsort-am | 22299.22 | 4539.26 | **+391.3%** | ✅ |
| 2,970 | dtsort-em | 22299.22 | 21260.16 | **+4.9%** | ✅ |
| 3,336 | dtsort-am | 25169.38 | 5199.30 | **+384.1%** | ✅ |
| 3,336 | dtsort-em | 25169.38 | 24650.70 | **+2.1%** | ✅ |
| 3,748 | dtsort-am | 28204.77 | 5840.98 | **+382.9%** | ✅ |
| 3,748 | dtsort-em | 28204.77 | 28597.42 | -1.4% | ⚠️ |
| 4,210 | dtsort-am | 33948.50 | 6812.76 | **+398.3%** | ✅ |
| 4,210 | dtsort-em | 33948.50 | 32582.70 | **+4.2%** | ✅ |
| 4,729 | dtsort-am | 43196.63 | 8019.96 | **+438.6%** | ✅ |
| 4,729 | dtsort-em | 43196.63 | 38239.29 | **+13.0%** | ✅ |
| 5,312 | dtsort-am | 49151.41 | 9151.60 | **+437.1%** | ✅ |
| 5,312 | dtsort-em | 49151.41 | 43887.56 | **+12.0%** | ✅ |
| 5,967 | dtsort-am | 54465.22 | 10563.83 | **+415.6%** | ✅ |
| 5,967 | dtsort-em | 54465.22 | 48837.00 | **+11.5%** | ✅ |
| 6,703 | dtsort-am | 61384.47 | 12348.02 | **+397.1%** | ✅ |
| 6,703 | dtsort-em | 61384.47 | 56223.25 | **+9.2%** | ✅ |
| 7,530 | dtsort-am | 69137.48 | 14223.71 | **+386.1%** | ✅ |
| 7,530 | dtsort-em | 69137.48 | 64698.82 | **+6.9%** | ✅ |
| 8,458 | dtsort-am | 78002.06 | 16155.88 | **+382.8%** | ✅ |
| 8,458 | dtsort-em | 78002.06 | 73168.06 | **+6.6%** | ✅ |
| 9,502 | dtsort-am | 87405.37 | 18570.08 | **+370.7%** | ✅ |
| 9,502 | dtsort-em | 87405.37 | 83953.72 | **+4.1%** | ✅ |
| 10,673 | dtsort-am | 98893.03 | 21138.07 | **+367.8%** | ✅ |
| 10,673 | dtsort-em | 98893.03 | 95530.67 | **+3.5%** | ✅ |
| 11,990 | dtsort-am | 110353.76 | 24142.46 | **+357.1%** | ✅ |
| 11,990 | dtsort-em | 110353.76 | 108302.07 | **+1.9%** | ✅ |
| 13,468 | dtsort-am | 123098.54 | 27754.59 | **+343.5%** | ✅ |
| 13,468 | dtsort-em | 123098.54 | 124683.87 | -1.3% | ⚠️ |
| 15,129 | dtsort-am | 138106.01 | 30819.72 | **+348.1%** | ✅ |
| 15,129 | dtsort-em | 138106.01 | 143339.67 | -3.7% | ⚠️ |
| 16,995 | dtsort-am | 165344.66 | 34992.05 | **+372.5%** | ✅ |
| 16,995 | dtsort-em | 165344.66 | 162517.08 | **+1.7%** | ✅ |
| 19,091 | dtsort-am | 204863.22 | 39954.28 | **+412.7%** | ✅ |
| 19,091 | dtsort-em | 204863.22 | 189152.70 | **+8.3%** | ✅ |
| 21,445 | dtsort-am | 228346.16 | 44853.02 | **+409.1%** | ✅ |
| 21,445 | dtsort-em | 228346.16 | 214283.04 | **+6.6%** | ✅ |
| 24,090 | dtsort-am | 255724.74 | 51074.04 | **+400.7%** | ✅ |
| 24,090 | dtsort-em | 255724.74 | 240844.87 | **+6.2%** | ✅ |
| 27,061 | dtsort-am | 287092.63 | 57985.20 | **+395.1%** | ✅ |
| 27,061 | dtsort-em | 287092.63 | 273932.60 | **+4.8%** | ✅ |
| 30,398 | dtsort-am | 327350.04 | 64003.04 | **+411.5%** | ✅ |
| 30,398 | dtsort-em | 327350.04 | 315001.85 | **+3.9%** | ✅ |
| 34,147 | dtsort-am | 361019.53 | 71925.07 | **+401.9%** | ✅ |
| 34,147 | dtsort-em | 361019.53 | 352822.01 | **+2.3%** | ✅ |
| 38,358 | dtsort-am | 406470.05 | 81868.02 | **+396.5%** | ✅ |
| 38,358 | dtsort-em | 406470.05 | 404297.21 | **+0.5%** | ✅ |
| 43,089 | dtsort-am | 457141.07 | 92585.96 | **+393.7%** | ✅ |
| 43,089 | dtsort-em | 457141.07 | 459423.86 | -0.5% | ✅ |
| 48,403 | dtsort-am | 514275.25 | 106768.28 | **+381.7%** | ✅ |
| 48,403 | dtsort-em | 514275.25 | 521483.74 | -1.4% | ⚠️ |
| 54,372 | dtsort-am | 575841.15 | 121290.71 | **+374.8%** | ✅ |
| 54,372 | dtsort-em | 575841.15 | 599268.78 | -3.9% | ⚠️ |
| 61,077 | dtsort-am | 661825.98 | 136980.31 | **+383.2%** | ✅ |
| 61,077 | dtsort-em | 661825.98 | 691016.64 | -4.2% | ⚠️ |
| 68,609 | dtsort-am | 785056.99 | 155478.05 | **+404.9%** | ✅ |
| 68,609 | dtsort-em | 785056.99 | 787448.26 | -0.3% | ✅ |
| 77,071 | dtsort-am | 950685.11 | 179661.77 | **+429.2%** | ✅ |
| 77,071 | dtsort-em | 950685.11 | 910852.76 | **+4.4%** | ✅ |
| 86,575 | dtsort-am | 1040235.04 | 202428.10 | **+413.9%** | ✅ |
| 86,575 | dtsort-em | 1040235.04 | 1027155.79 | **+1.3%** | ✅ |
| 97,252 | dtsort-am | 1185347.81 | 228595.76 | **+418.5%** | ✅ |
| 97,252 | dtsort-em | 1185347.81 | 1151020.18 | **+3.0%** | ✅ |
| 109,246 | dtsort-am | 1317427.38 | 260915.43 | **+404.9%** | ✅ |
| 109,246 | dtsort-em | 1317427.38 | 1295287.35 | **+1.7%** | ✅ |
| 122,718 | dtsort-am | 1491788.56 | 284799.68 | **+423.8%** | ✅ |
| 122,718 | dtsort-em | 1491788.56 | 1493299.32 | -0.1% | ✅ |
| 137,852 | dtsort-am | 1666645.14 | 325028.27 | **+412.8%** | ✅ |
| 137,852 | dtsort-em | 1666645.14 | 1686962.66 | -1.2% | ⚠️ |
| 154,853 | dtsort-am | 1878997.21 | 364200.61 | **+415.9%** | ✅ |
| 154,853 | dtsort-em | 1878997.21 | 1935448.32 | -2.9% | ⚠️ |
| 173,950 | dtsort-am | 2089650.99 | 415678.62 | **+402.7%** | ✅ |
| 173,950 | dtsort-em | 2089650.99 | 2178096.18 | -4.1% | ⚠️ |
| 195,402 | dtsort-am | 2377975.34 | 476920.23 | **+398.6%** | ✅ |
| 195,402 | dtsort-em | 2377975.34 | 2481614.95 | -4.2% | ⚠️ |
| 219,500 | dtsort-am | 2694892.31 | 543395.37 | **+395.9%** | ✅ |
| 219,500 | dtsort-em | 2694892.31 | 2846565.72 | -5.3% | ⚠️ |
| 246,569 | dtsort-am | 2990519.05 | 606725.41 | **+392.9%** | ✅ |
| 246,569 | dtsort-em | 2990519.05 | 3240930.75 | -7.7% | ⚠️ |
| 276,977 | dtsort-am | 3658155.54 | 696509.82 | **+425.2%** | ✅ |
| 276,977 | dtsort-em | 3658155.54 | 3722740.91 | -1.7% | ⚠️ |
| 311,135 | dtsort-am | 4267097.47 | 804180.47 | **+430.6%** | ✅ |
| 311,135 | dtsort-em | 4267097.47 | 4303498.98 | -0.8% | ✅ |
| 349,506 | dtsort-am | 4760023.85 | 903490.36 | **+426.8%** | ✅ |
| 349,506 | dtsort-em | 4760023.85 | 4839842.24 | -1.6% | ⚠️ |
| 392,608 | dtsort-am | 5362779.26 | 1025031.47 | **+423.2%** | ✅ |
| 392,608 | dtsort-em | 5362779.26 | 5426054.26 | -1.2% | ⚠️ |
| 441,026 | dtsort-am | 5984486.59 | 1157418.17 | **+417.1%** | ✅ |
| 441,026 | dtsort-em | 5984486.59 | 6162699.56 | -2.9% | ⚠️ |
| 495,415 | dtsort-am | 6810328.43 | 1285861.52 | **+429.6%** | ✅ |
| 495,415 | dtsort-em | 6810328.43 | 7047251.69 | -3.4% | ⚠️ |
| 556,512 | dtsort-am | 7539915.32 | 1439374.22 | **+423.8%** | ✅ |
| 556,512 | dtsort-em | 7539915.32 | 7917938.20 | -4.8% | ⚠️ |
| 625,143 | dtsort-am | 8517426.95 | 1632643.60 | **+421.7%** | ✅ |
| 625,143 | dtsort-em | 8517426.95 | 9063433.99 | -6.0% | ⚠️ |
| 702,238 | dtsort-am | 9611261.00 | 1861683.55 | **+416.3%** | ✅ |
| 702,238 | dtsort-em | 9611261.00 | 10205370.17 | -5.8% | ⚠️ |
| 788,841 | dtsort-am | 10820758.34 | 2131784.17 | **+407.6%** | ✅ |
| 788,841 | dtsort-em | 10820758.34 | 11595124.30 | -6.7% | ⚠️ |
| 886,124 | dtsort-am | 12101571.84 | 2424513.94 | **+399.1%** | ✅ |
| 886,124 | dtsort-em | 12101571.84 | 13228137.56 | -8.5% | ⚠️ |
| 995,405 | dtsort-am | 13699138.90 | 2718047.48 | **+404.0%** | ✅ |
| 995,405 | dtsort-em | 13699138.90 | 15069597.83 | -9.1% | ⚠️ |
| 1,118,162 | dtsort-am | 16543723.22 | 3111752.95 | **+431.7%** | ✅ |
| 1,118,162 | dtsort-em | 16543723.22 | 17228463.41 | -4.0% | ⚠️ |
| 1,256,058 | dtsort-am | 19102520.27 | 3612497.23 | **+428.8%** | ✅ |
| 1,256,058 | dtsort-em | 19102520.27 | 19754055.94 | -3.3% | ⚠️ |
| 1,410,960 | dtsort-am | 21425214.63 | 4100930.15 | **+422.4%** | ✅ |
| 1,410,960 | dtsort-em | 21425214.63 | 22184236.97 | -3.4% | ⚠️ |
| 1,584,966 | dtsort-am | 24075653.73 | 4658814.88 | **+416.8%** | ✅ |
| 1,584,966 | dtsort-em | 24075653.73 | 25026958.35 | -3.8% | ⚠️ |
| 1,780,430 | dtsort-am | 27021012.84 | 5290345.46 | **+410.8%** | ✅ |
| 1,780,430 | dtsort-em | 27021012.84 | 28466978.35 | -5.1% | ⚠️ |
| 2,000,000 | dtsort-am | 30373909.43 | 5855186.97 | **+418.8%** | ✅ |
| 2,000,000 | dtsort-em | 30373909.43 | 32200924.23 | -5.7% | ⚠️ |

#### dt (WideRow)
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort-am | 214.52 | 199.46 | **+7.5%** | ✅ |
| 2 | dtsort-em | 214.52 | 196.21 | **+9.3%** | ✅ |
| 3 | dtsort-am | 368.96 | 270.17 | **+36.6%** | ✅ |
| 3 | dtsort-em | 368.96 | 268.63 | **+37.3%** | ✅ |
| 4 | dtsort-am | 501.78 | 349.75 | **+43.5%** | ✅ |
| 4 | dtsort-em | 501.78 | 350.29 | **+43.2%** | ✅ |
| 5 | dtsort-am | 727.91 | 462.73 | **+57.3%** | ✅ |
| 5 | dtsort-em | 727.91 | 471.95 | **+54.2%** | ✅ |
| 6 | dtsort-am | 788.74 | 802.64 | -1.7% | ⚠️ |
| 6 | dtsort-em | 788.74 | 813.76 | -3.1% | ⚠️ |
| 7 | dtsort-am | 908.32 | 789.44 | **+15.1%** | ✅ |
| 7 | dtsort-em | 908.32 | 785.27 | **+15.7%** | ✅ |
| 8 | dtsort-am | 1311.22 | 892.38 | **+46.9%** | ✅ |
| 8 | dtsort-em | 1311.22 | 887.81 | **+47.7%** | ✅ |
| 9 | dtsort-am | 1763.36 | 1010.78 | **+74.5%** | ✅ |
| 9 | dtsort-em | 1763.36 | 1006.86 | **+75.1%** | ✅ |
| 10 | dtsort-am | 2034.39 | 1235.51 | **+64.7%** | ✅ |
| 10 | dtsort-em | 2034.39 | 1231.94 | **+65.1%** | ✅ |
| 20 | dtsort-am | 6862.55 | 5216.27 | **+31.6%** | ✅ |
| 20 | dtsort-em | 6862.55 | 5156.10 | **+33.1%** | ✅ |
| 22 | dtsort-am | 7868.86 | 5724.96 | **+37.4%** | ✅ |
| 22 | dtsort-em | 7868.86 | 5674.63 | **+38.7%** | ✅ |
| 25 | dtsort-am | 9251.42 | 6580.18 | **+40.6%** | ✅ |
| 25 | dtsort-em | 9251.42 | 6590.27 | **+40.4%** | ✅ |
| 28 | dtsort-am | 10568.10 | 7240.10 | **+46.0%** | ✅ |
| 28 | dtsort-em | 10568.10 | 7197.75 | **+46.8%** | ✅ |
| 32 | dtsort-am | 12229.40 | 8313.75 | **+47.1%** | ✅ |
| 32 | dtsort-em | 12229.40 | 8334.19 | **+46.7%** | ✅ |
| 36 | dtsort-am | 14286.00 | 9315.19 | **+53.4%** | ✅ |
| 36 | dtsort-em | 14286.00 | 9276.28 | **+54.0%** | ✅ |
| 40 | dtsort-am | 16419.96 | 10285.50 | **+59.6%** | ✅ |
| 40 | dtsort-em | 16419.96 | 10222.36 | **+60.6%** | ✅ |
| 45 | dtsort-am | 19070.86 | 11599.33 | **+64.4%** | ✅ |
| 45 | dtsort-em | 19070.86 | 11606.90 | **+64.3%** | ✅ |
| 51 | dtsort-am | 22221.18 | 13156.44 | **+68.9%** | ✅ |
| 51 | dtsort-em | 22221.18 | 13148.38 | **+69.0%** | ✅ |
| 57 | dtsort-am | 24862.89 | 14758.43 | **+68.5%** | ✅ |
| 57 | dtsort-em | 24862.89 | 14703.10 | **+69.1%** | ✅ |
| 64 | dtsort-am | 28220.76 | 16468.90 | **+71.4%** | ✅ |
| 64 | dtsort-em | 28220.76 | 16494.51 | **+71.1%** | ✅ |
| 72 | dtsort-am | 34162.24 | 18641.38 | **+83.3%** | ✅ |
| 72 | dtsort-em | 34162.24 | 18564.05 | **+84.0%** | ✅ |
| 81 | dtsort-am | 40036.45 | 20911.07 | **+91.5%** | ✅ |
| 81 | dtsort-em | 40036.45 | 21010.96 | **+90.6%** | ✅ |
| 91 | dtsort-am | 45887.65 | 23758.75 | **+93.1%** | ✅ |
| 91 | dtsort-em | 45887.65 | 23726.43 | **+93.4%** | ✅ |
| 102 | dtsort-am | 52808.35 | 26548.58 | **+98.9%** | ✅ |
| 102 | dtsort-em | 52808.35 | 26417.98 | **+99.9%** | ✅ |
| 114 | dtsort-am | 59392.86 | 29692.80 | **+100.0%** | ✅ |
| 114 | dtsort-em | 59392.86 | 29819.47 | **+99.2%** | ✅ |
| 129 | dtsort-am | 69128.29 | 33531.86 | **+106.2%** | ✅ |
| 129 | dtsort-em | 69128.29 | 33691.03 | **+105.2%** | ✅ |
| 144 | dtsort-am | 78503.41 | 37419.26 | **+109.8%** | ✅ |
| 144 | dtsort-em | 78503.41 | 37654.24 | **+108.5%** | ✅ |
| 162 | dtsort-am | 91102.73 | 42185.33 | **+116.0%** | ✅ |
| 162 | dtsort-em | 91102.73 | 42462.68 | **+114.5%** | ✅ |
| 182 | dtsort-am | 103813.22 | 47502.22 | **+118.5%** | ✅ |
| 182 | dtsort-em | 103813.22 | 47858.77 | **+116.9%** | ✅ |
| 205 | dtsort-am | 120885.90 | 53674.53 | **+125.2%** | ✅ |
| 205 | dtsort-em | 120885.90 | 53873.43 | **+124.4%** | ✅ |
| 230 | dtsort-am | 137608.07 | 60296.57 | **+128.2%** | ✅ |
| 230 | dtsort-em | 137608.07 | 60644.44 | **+126.9%** | ✅ |
| 258 | dtsort-am | 158447.33 | 68071.91 | **+132.8%** | ✅ |
| 258 | dtsort-em | 158447.33 | 67820.57 | **+133.6%** | ✅ |
| 290 | dtsort-am | 186529.17 | 76621.79 | **+143.4%** | ✅ |
| 290 | dtsort-em | 186529.17 | 77076.19 | **+142.0%** | ✅ |
| 326 | dtsort-am | 213933.37 | 110689.04 | **+93.3%** | ✅ |
| 326 | dtsort-em | 213933.37 | 110696.86 | **+93.3%** | ✅ |
| 366 | dtsort-am | 243351.34 | 151533.30 | **+60.6%** | ✅ |
| 366 | dtsort-em | 243351.34 | 151593.82 | **+60.5%** | ✅ |
| 411 | dtsort-am | 279823.89 | 170464.28 | **+64.2%** | ✅ |
| 411 | dtsort-em | 279823.89 | 171228.20 | **+63.4%** | ✅ |
| 462 | dtsort-am | 318772.42 | 191432.92 | **+66.5%** | ✅ |
| 462 | dtsort-em | 318772.42 | 193951.85 | **+64.4%** | ✅ |
| 519 | dtsort-am | 371159.61 | 216289.48 | **+71.6%** | ✅ |
| 519 | dtsort-em | 371159.61 | 217416.62 | **+70.7%** | ✅ |
| 583 | dtsort-am | 444912.39 | 243772.05 | **+82.5%** | ✅ |
| 583 | dtsort-em | 444912.39 | 245284.63 | **+81.4%** | ✅ |
| 655 | dtsort-am | 484653.56 | 274190.59 | **+76.8%** | ✅ |
| 655 | dtsort-em | 484653.56 | 276676.69 | **+75.2%** | ✅ |
| 736 | dtsort-am | 569949.78 | 307871.63 | **+85.1%** | ✅ |
| 736 | dtsort-em | 569949.78 | 310135.02 | **+83.8%** | ✅ |
| 826 | dtsort-am | 624533.83 | 345774.96 | **+80.6%** | ✅ |
| 826 | dtsort-em | 624533.83 | 347505.62 | **+79.7%** | ✅ |
| 928 | dtsort-am | 731302.32 | 390148.92 | **+87.4%** | ✅ |
| 928 | dtsort-em | 731302.32 | 393682.78 | **+85.8%** | ✅ |
| 1,043 | dtsort-am | 813081.89 | 441193.78 | **+84.3%** | ✅ |
| 1,043 | dtsort-em | 813081.89 | 445681.74 | **+82.4%** | ✅ |
| 1,171 | dtsort-am | 962296.09 | 498256.13 | **+93.1%** | ✅ |
| 1,171 | dtsort-em | 962296.09 | 498890.41 | **+92.9%** | ✅ |
| 1,316 | dtsort-am | 1092297.83 | 643786.04 | **+69.7%** | ✅ |
| 1,316 | dtsort-em | 1092297.83 | 650528.14 | **+67.9%** | ✅ |
| 1,478 | dtsort-am | 1254479.36 | 873158.95 | **+43.7%** | ✅ |
| 1,478 | dtsort-em | 1254479.36 | 865601.63 | **+44.9%** | ✅ |
| 1,660 | dtsort-am | 1422547.64 | 968753.15 | **+46.8%** | ✅ |
| 1,660 | dtsort-em | 1422547.64 | 993990.69 | **+43.1%** | ✅ |
| 1,865 | dtsort-am | 1646141.36 | 1101577.93 | **+49.4%** | ✅ |
| 1,865 | dtsort-em | 1646141.36 | 1111171.09 | **+48.1%** | ✅ |
| 2,095 | dtsort-am | 1863405.32 | 1254224.71 | **+48.6%** | ✅ |
| 2,095 | dtsort-em | 1863405.32 | 1253111.01 | **+48.7%** | ✅ |
| 2,354 | dtsort-am | 2134328.46 | 1438658.05 | **+48.4%** | ✅ |
| 2,354 | dtsort-em | 2134328.46 | 1442669.80 | **+47.9%** | ✅ |
| 2,644 | dtsort-am | 2432321.94 | 1650759.34 | **+47.3%** | ✅ |
| 2,644 | dtsort-em | 2432321.94 | 1657209.32 | **+46.8%** | ✅ |
| 2,970 | dtsort-am | 2801473.58 | 1915299.20 | **+46.3%** | ✅ |
| 2,970 | dtsort-em | 2801473.58 | 1920143.74 | **+45.9%** | ✅ |
| 3,336 | dtsort-am | 3224813.18 | 2297121.91 | **+40.4%** | ✅ |
| 3,336 | dtsort-em | 3224813.18 | 2280165.85 | **+41.4%** | ✅ |
| 3,748 | dtsort-am | 3732218.02 | 2702560.86 | **+38.1%** | ✅ |
| 3,748 | dtsort-em | 3732218.02 | 2700092.50 | **+38.2%** | ✅ |
| 4,210 | dtsort-am | 4396099.15 | 3194279.09 | **+37.6%** | ✅ |
| 4,210 | dtsort-em | 4396099.15 | 3222051.18 | **+36.4%** | ✅ |
| 4,729 | dtsort-am | 5069600.82 | 3775276.58 | **+34.3%** | ✅ |
| 4,729 | dtsort-em | 5069600.82 | 3761192.38 | **+34.8%** | ✅ |
| 5,312 | dtsort-am | 5957335.17 | 4930203.27 | **+20.8%** | ✅ |
| 5,312 | dtsort-em | 5957335.17 | 4911601.95 | **+21.3%** | ✅ |
| 5,967 | dtsort-am | 6965135.51 | 6033545.29 | **+15.4%** | ✅ |
| 5,967 | dtsort-em | 6965135.51 | 6047164.49 | **+15.2%** | ✅ |
| 6,703 | dtsort-am | 7991079.45 | 6962009.17 | **+14.8%** | ✅ |
| 6,703 | dtsort-em | 7991079.45 | 6988356.43 | **+14.3%** | ✅ |
| 7,530 | dtsort-am | 9235449.56 | 7982040.25 | **+15.7%** | ✅ |
| 7,530 | dtsort-em | 9235449.56 | 8028386.84 | **+15.0%** | ✅ |
| 8,458 | dtsort-am | 10704101.01 | 9226842.10 | **+16.0%** | ✅ |
| 8,458 | dtsort-em | 10704101.01 | 9226421.61 | **+16.0%** | ✅ |
| 9,502 | dtsort-am | 12370228.43 | 10601986.12 | **+16.7%** | ✅ |
| 9,502 | dtsort-em | 12370228.43 | 10640738.64 | **+16.3%** | ✅ |
| 10,673 | dtsort-am | 14213788.34 | 12136856.33 | **+17.1%** | ✅ |
| 10,673 | dtsort-em | 14213788.34 | 12180747.12 | **+16.7%** | ✅ |
| 11,990 | dtsort-am | 16323622.09 | 13847011.43 | **+17.9%** | ✅ |
| 11,990 | dtsort-em | 16323622.09 | 13863519.18 | **+17.7%** | ✅ |
| 13,468 | dtsort-am | 18735458.31 | 15972825.40 | **+17.3%** | ✅ |
| 13,468 | dtsort-em | 18735458.31 | 15895272.73 | **+17.9%** | ✅ |
| 15,129 | dtsort-am | 21517847.66 | 18208113.24 | **+18.2%** | ✅ |
| 15,129 | dtsort-em | 21517847.66 | 18176891.03 | **+18.4%** | ✅ |
| 16,995 | dtsort-am | 24837352.68 | 20774180.14 | **+19.6%** | ✅ |
| 16,995 | dtsort-em | 24837352.68 | 20787506.34 | **+19.5%** | ✅ |
| 19,091 | dtsort-am | 28690058.32 | 23613888.86 | **+21.5%** | ✅ |
| 19,091 | dtsort-em | 28690058.32 | 23604205.57 | **+21.5%** | ✅ |
| 21,445 | dtsort-am | 32937402.80 | 29189015.62 | **+12.8%** | ✅ |
| 21,445 | dtsort-em | 32937402.80 | 29113979.17 | **+13.1%** | ✅ |
| 24,090 | dtsort-am | 37803894.74 | 33673262.50 | **+12.3%** | ✅ |
| 24,090 | dtsort-em | 37803894.74 | 33849734.14 | **+11.7%** | ✅ |
| 27,061 | dtsort-am | 42890218.76 | 38094004.62 | **+12.6%** | ✅ |
| 27,061 | dtsort-em | 42890218.76 | 38505423.61 | **+11.4%** | ✅ |
| 30,398 | dtsort-am | 48569032.71 | 42906557.31 | **+13.2%** | ✅ |
| 30,398 | dtsort-em | 48569032.71 | 43132830.75 | **+12.6%** | ✅ |
| 34,147 | dtsort-am | 54821513.90 | 48559093.01 | **+12.9%** | ✅ |
| 34,147 | dtsort-em | 54821513.90 | 48656131.01 | **+12.7%** | ✅ |
| 38,358 | dtsort-am | 62192695.82 | 54642868.00 | **+13.8%** | ✅ |
| 38,358 | dtsort-em | 62192695.82 | 54768423.57 | **+13.6%** | ✅ |
| 43,089 | dtsort-am | 70262541.68 | 61674458.36 | **+13.9%** | ✅ |
| 43,089 | dtsort-em | 70262541.68 | 61813265.11 | **+13.7%** | ✅ |
| 48,403 | dtsort-am | 79616255.13 | 69551287.01 | **+14.5%** | ✅ |
| 48,403 | dtsort-em | 79616255.13 | 69912199.11 | **+13.9%** | ✅ |
| 54,372 | dtsort-am | 90390779.71 | 79262505.26 | **+14.0%** | ✅ |
| 54,372 | dtsort-em | 90390779.71 | 79070041.77 | **+14.3%** | ✅ |
| 61,077 | dtsort-am | 102587472.14 | 89225357.17 | **+15.0%** | ✅ |
| 61,077 | dtsort-em | 102587472.14 | 89591273.71 | **+14.5%** | ✅ |
| 68,609 | dtsort-am | 117590409.66 | 101064368.00 | **+16.4%** | ✅ |
| 68,609 | dtsort-em | 117590409.66 | 101328416.69 | **+16.0%** | ✅ |
| 77,071 | dtsort-am | 135341833.44 | 114097298.47 | **+18.6%** | ✅ |
| 77,071 | dtsort-em | 135341833.44 | 114544069.51 | **+18.2%** | ✅ |
| 86,575 | dtsort-am | 154693718.77 | 140767739.50 | **+9.9%** | ✅ |
| 86,575 | dtsort-em | 154693718.77 | 141071874.97 | **+9.7%** | ✅ |
| 97,252 | dtsort-am | 177044437.50 | 160401562.46 | **+10.4%** | ✅ |
| 97,252 | dtsort-em | 177044437.50 | 160603864.47 | **+10.2%** | ✅ |
| 109,246 | dtsort-am | 200471764.04 | 181677513.68 | **+10.3%** | ✅ |
| 109,246 | dtsort-em | 200471764.04 | 183491472.39 | **+9.3%** | ✅ |
| 122,718 | dtsort-am | 229284847.31 | 205602166.63 | **+11.5%** | ✅ |
| 122,718 | dtsort-em | 229284847.31 | 206051791.71 | **+11.3%** | ✅ |
| 137,852 | dtsort-am | 263416541.51 | 232521749.96 | **+13.3%** | ✅ |
| 137,852 | dtsort-em | 263416541.51 | 233092208.32 | **+13.0%** | ✅ |
| 154,853 | dtsort-am | 293350083.52 | 263027354.49 | **+11.5%** | ✅ |
| 154,853 | dtsort-em | 293350083.52 | 263646979.00 | **+11.3%** | ✅ |
| 173,950 | dtsort-am | 331584124.94 | 297044271.02 | **+11.6%** | ✅ |
| 173,950 | dtsort-em | 331584124.94 | 298377937.52 | **+11.1%** | ✅ |
| 195,402 | dtsort-am | 375925333.47 | 335520187.39 | **+12.0%** | ✅ |
| 195,402 | dtsort-em | 375925333.47 | 336181812.57 | **+11.8%** | ✅ |
| 219,500 | dtsort-am | 426397666.57 | 381722958.06 | **+11.7%** | ✅ |
| 219,500 | dtsort-em | 426397666.57 | 380412583.01 | **+12.1%** | ✅ |
| 246,569 | dtsort-am | 483687917.00 | 430064228.95 | **+12.5%** | ✅ |
| 246,569 | dtsort-em | 483687917.00 | 430702395.97 | **+12.3%** | ✅ |
| 276,977 | dtsort-am | 553089916.01 | 486834208.02 | **+13.6%** | ✅ |
| 276,977 | dtsort-em | 553089916.01 | 487349000.06 | **+13.5%** | ✅ |
| 311,135 | dtsort-am | 633849875.07 | 548234499.99 | **+15.6%** | ✅ |
| 311,135 | dtsort-em | 633849875.07 | 549183625.03 | **+15.4%** | ✅ |
| 349,506 | dtsort-am | 723735916.89 | 672246541.83 | **+7.7%** | ✅ |
| 349,506 | dtsort-em | 723735916.89 | 673042875.02 | **+7.5%** | ✅ |
| 392,608 | dtsort-am | 833167500.10 | 757415249.94 | **+10.0%** | ✅ |
| 392,608 | dtsort-em | 833167500.10 | 759909625.17 | **+9.6%** | ✅ |
| 441,026 | dtsort-am | 931300207.97 | 858308749.97 | **+8.5%** | ✅ |
| 441,026 | dtsort-em | 931300207.97 | 860850624.97 | **+8.2%** | ✅ |
| 495,415 | dtsort-am | 1062297125.11 | 968354500.13 | **+9.7%** | ✅ |
| 495,415 | dtsort-em | 1062297125.11 | 969862249.91 | **+9.5%** | ✅ |
| 556,512 | dtsort-am | 1198567083.11 | 1094260417.16 | **+9.5%** | ✅ |
| 556,512 | dtsort-em | 1198567083.11 | 1093857917.00 | **+9.6%** | ✅ |
| 625,143 | dtsort-am | 1357056666.86 | 1245250334.03 | **+9.0%** | ✅ |
| 625,143 | dtsort-em | 1357056666.86 | 1237333667.00 | **+9.7%** | ✅ |
| 702,238 | dtsort-am | 1538714124.82 | 1395872582.91 | **+10.2%** | ✅ |
| 702,238 | dtsort-em | 1538714124.82 | 1398262457.92 | **+10.0%** | ✅ |
| 788,841 | dtsort-am | 1744876125.13 | 1576094500.02 | **+10.7%** | ✅ |
| 788,841 | dtsort-em | 1744876125.13 | 1578608209.03 | **+10.5%** | ✅ |
| 886,124 | dtsort-am | 1969506500.06 | 1781267750.08 | **+10.6%** | ✅ |
| 886,124 | dtsort-em | 1969506500.06 | 1785145916.04 | **+10.3%** | ✅ |
| 995,405 | dtsort-am | 2231348792.09 | 2015425790.80 | **+10.7%** | ✅ |
| 995,405 | dtsort-em | 2231348792.09 | 2022511500.17 | **+10.3%** | ✅ |
| 1,118,162 | dtsort-am | 2863027709.07 | 2457166707.84 | **+16.5%** | ✅ |
| 1,118,162 | dtsort-em | 2863027709.07 | 2532810500.12 | **+13.0%** | ✅ |
| 1,256,058 | dtsort-am | 3256990000.14 | 2829548500.01 | **+15.1%** | ✅ |
| 1,256,058 | dtsort-em | 3256990000.14 | 2847431457.83 | **+14.4%** | ✅ |
| 1,410,960 | dtsort-am | 3689154249.85 | 3407984958.03 | **+8.3%** | ✅ |
| 1,410,960 | dtsort-em | 3689154249.85 | 3422080624.85 | **+7.8%** | ✅ |
| 1,584,966 | dtsort-am | 4182996333.11 | 3839397166.86 | **+8.9%** | ✅ |
| 1,584,966 | dtsort-em | 4182996333.11 | 3813689999.98 | **+9.7%** | ✅ |
| 1,780,430 | dtsort-am | 4755862708.90 | 4340965582.99 | **+9.6%** | ✅ |
| 1,780,430 | dtsort-em | 4755862708.90 | 4349527208.13 | **+9.3%** | ✅ |
| 2,000,000 | dtsort-am | 5407449249.88 | 4900332083.23 | **+10.3%** | ✅ |
| 2,000,000 | dtsort-em | 5407449249.88 | 4914486333.96 | **+10.0%** | ✅ |

#### dt (WideRow (Reverse))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort-am | 214.68 | 199.23 | **+7.8%** | ✅ |
| 2 | dtsort-em | 214.68 | 195.90 | **+9.6%** | ✅ |
| 3 | dtsort-am | 368.96 | 270.07 | **+36.6%** | ✅ |
| 3 | dtsort-em | 368.96 | 268.27 | **+37.5%** | ✅ |
| 4 | dtsort-am | 488.43 | 351.50 | **+39.0%** | ✅ |
| 4 | dtsort-em | 488.43 | 348.53 | **+40.1%** | ✅ |
| 5 | dtsort-am | 759.93 | 400.02 | **+90.0%** | ✅ |
| 5 | dtsort-em | 759.93 | 389.14 | **+95.3%** | ✅ |
| 6 | dtsort-am | 917.97 | 731.89 | **+25.4%** | ✅ |
| 6 | dtsort-em | 917.97 | 736.66 | **+24.6%** | ✅ |
| 7 | dtsort-am | 1173.48 | 851.13 | **+37.9%** | ✅ |
| 7 | dtsort-em | 1173.48 | 848.93 | **+38.2%** | ✅ |
| 8 | dtsort-am | 1420.80 | 1124.24 | **+26.4%** | ✅ |
| 8 | dtsort-em | 1420.80 | 1120.97 | **+26.7%** | ✅ |
| 9 | dtsort-am | 1862.35 | 1237.31 | **+50.5%** | ✅ |
| 9 | dtsort-em | 1862.35 | 1231.43 | **+51.2%** | ✅ |
| 10 | dtsort-am | 2202.64 | 1462.46 | **+50.6%** | ✅ |
| 10 | dtsort-em | 2202.64 | 1459.37 | **+50.9%** | ✅ |
| 20 | dtsort-am | 7184.19 | 5396.01 | **+33.1%** | ✅ |
| 20 | dtsort-em | 7184.19 | 5436.02 | **+32.2%** | ✅ |
| 22 | dtsort-am | 8243.52 | 5886.32 | **+40.0%** | ✅ |
| 22 | dtsort-em | 8243.52 | 5929.20 | **+39.0%** | ✅ |
| 25 | dtsort-am | 9482.45 | 6605.15 | **+43.6%** | ✅ |
| 25 | dtsort-em | 9482.45 | 6674.47 | **+42.1%** | ✅ |
| 28 | dtsort-am | 10785.91 | 7356.59 | **+46.6%** | ✅ |
| 28 | dtsort-em | 10785.91 | 7428.18 | **+45.2%** | ✅ |
| 32 | dtsort-am | 12708.93 | 8339.31 | **+52.4%** | ✅ |
| 32 | dtsort-em | 12708.93 | 8425.30 | **+50.8%** | ✅ |
| 36 | dtsort-am | 15175.62 | 9385.77 | **+61.7%** | ✅ |
| 36 | dtsort-em | 15175.62 | 9482.61 | **+60.0%** | ✅ |
| 40 | dtsort-am | 17179.14 | 10386.08 | **+65.4%** | ✅ |
| 40 | dtsort-em | 17179.14 | 10414.49 | **+65.0%** | ✅ |
| 45 | dtsort-am | 20420.61 | 11701.72 | **+74.5%** | ✅ |
| 45 | dtsort-em | 20420.61 | 11739.11 | **+74.0%** | ✅ |
| 51 | dtsort-am | 24295.64 | 13204.86 | **+84.0%** | ✅ |
| 51 | dtsort-em | 24295.64 | 13245.63 | **+83.4%** | ✅ |
| 57 | dtsort-am | 28057.74 | 14580.48 | **+92.4%** | ✅ |
| 57 | dtsort-em | 28057.74 | 14685.88 | **+91.1%** | ✅ |
| 64 | dtsort-am | 32301.42 | 16371.55 | **+97.3%** | ✅ |
| 64 | dtsort-em | 32301.42 | 16375.78 | **+97.3%** | ✅ |
| 72 | dtsort-am | 34524.70 | 18337.73 | **+88.3%** | ✅ |
| 72 | dtsort-em | 34524.70 | 18408.41 | **+87.5%** | ✅ |
| 81 | dtsort-am | 39969.22 | 20584.78 | **+94.2%** | ✅ |
| 81 | dtsort-em | 39969.22 | 20713.86 | **+93.0%** | ✅ |
| 91 | dtsort-am | 47132.37 | 23298.96 | **+102.3%** | ✅ |
| 91 | dtsort-em | 47132.37 | 23419.81 | **+101.3%** | ✅ |
| 102 | dtsort-am | 54287.88 | 26314.82 | **+106.3%** | ✅ |
| 102 | dtsort-em | 54287.88 | 26457.88 | **+105.2%** | ✅ |
| 114 | dtsort-am | 62238.70 | 29205.36 | **+113.1%** | ✅ |
| 114 | dtsort-em | 62238.70 | 29458.68 | **+111.3%** | ✅ |
| 129 | dtsort-am | 70873.84 | 32782.05 | **+116.2%** | ✅ |
| 129 | dtsort-em | 70873.84 | 32958.70 | **+115.0%** | ✅ |
| 144 | dtsort-am | 81183.90 | 36718.26 | **+121.1%** | ✅ |
| 144 | dtsort-em | 81183.90 | 37094.61 | **+118.9%** | ✅ |
| 162 | dtsort-am | 94835.39 | 41071.03 | **+130.9%** | ✅ |
| 162 | dtsort-em | 94835.39 | 41387.31 | **+129.1%** | ✅ |
| 182 | dtsort-am | 110475.04 | 46312.35 | **+138.5%** | ✅ |
| 182 | dtsort-em | 110475.04 | 46654.20 | **+136.8%** | ✅ |
| 205 | dtsort-am | 129219.77 | 52637.70 | **+145.5%** | ✅ |
| 205 | dtsort-em | 129219.77 | 52614.99 | **+145.6%** | ✅ |
| 230 | dtsort-am | 149141.99 | 58348.68 | **+155.6%** | ✅ |
| 230 | dtsort-em | 149141.99 | 58497.36 | **+155.0%** | ✅ |
| 258 | dtsort-am | 170099.17 | 65349.61 | **+160.3%** | ✅ |
| 258 | dtsort-em | 170099.17 | 65948.05 | **+157.9%** | ✅ |
| 290 | dtsort-am | 184805.53 | 73713.43 | **+150.7%** | ✅ |
| 290 | dtsort-em | 184805.53 | 73981.97 | **+149.8%** | ✅ |
| 326 | dtsort-am | 215188.00 | 107798.47 | **+99.6%** | ✅ |
| 326 | dtsort-em | 215188.00 | 108248.53 | **+98.8%** | ✅ |
| 366 | dtsort-am | 248166.55 | 147784.86 | **+67.9%** | ✅ |
| 366 | dtsort-em | 248166.55 | 149230.92 | **+66.3%** | ✅ |
| 411 | dtsort-am | 283873.37 | 167320.59 | **+69.7%** | ✅ |
| 411 | dtsort-em | 283873.37 | 168593.36 | **+68.4%** | ✅ |
| 462 | dtsort-am | 321624.53 | 187818.93 | **+71.2%** | ✅ |
| 462 | dtsort-em | 321624.53 | 188691.44 | **+70.4%** | ✅ |
| 519 | dtsort-am | 380959.37 | 216310.60 | **+76.1%** | ✅ |
| 519 | dtsort-em | 380959.37 | 215160.73 | **+77.1%** | ✅ |
| 583 | dtsort-am | 434186.01 | 238047.82 | **+82.4%** | ✅ |
| 583 | dtsort-em | 434186.01 | 239068.81 | **+81.6%** | ✅ |
| 655 | dtsort-am | 495750.06 | 267457.22 | **+85.4%** | ✅ |
| 655 | dtsort-em | 495750.06 | 268874.34 | **+84.4%** | ✅ |
| 736 | dtsort-am | 561299.85 | 297900.60 | **+88.4%** | ✅ |
| 736 | dtsort-em | 561299.85 | 300531.14 | **+86.8%** | ✅ |
| 826 | dtsort-am | 650622.54 | 336471.60 | **+93.4%** | ✅ |
| 826 | dtsort-em | 650622.54 | 337811.16 | **+92.6%** | ✅ |
| 928 | dtsort-am | 747360.17 | 376483.17 | **+98.5%** | ✅ |
| 928 | dtsort-em | 747360.17 | 383505.29 | **+94.9%** | ✅ |
| 1,043 | dtsort-am | 859666.67 | 432524.19 | **+98.8%** | ✅ |
| 1,043 | dtsort-em | 859666.67 | 431322.21 | **+99.3%** | ✅ |
| 1,171 | dtsort-am | 960752.12 | 486148.90 | **+97.6%** | ✅ |
| 1,171 | dtsort-em | 960752.12 | 490226.81 | **+96.0%** | ✅ |
| 1,316 | dtsort-am | 1091676.45 | 622400.41 | **+75.4%** | ✅ |
| 1,316 | dtsort-em | 1091676.45 | 628220.76 | **+73.8%** | ✅ |
| 1,478 | dtsort-am | 1261171.90 | 837181.08 | **+50.6%** | ✅ |
| 1,478 | dtsort-em | 1261171.90 | 844202.01 | **+49.4%** | ✅ |
| 1,660 | dtsort-am | 1434585.42 | 947062.01 | **+51.5%** | ✅ |
| 1,660 | dtsort-em | 1434585.42 | 956830.32 | **+49.9%** | ✅ |
| 1,865 | dtsort-am | 1701233.48 | 1079980.47 | **+57.5%** | ✅ |
| 1,865 | dtsort-em | 1701233.48 | 1087414.59 | **+56.4%** | ✅ |
| 2,095 | dtsort-am | 1905612.29 | 1227902.08 | **+55.2%** | ✅ |
| 2,095 | dtsort-em | 1905612.29 | 1231412.80 | **+54.8%** | ✅ |
| 2,354 | dtsort-am | 2155661.03 | 1394638.05 | **+54.6%** | ✅ |
| 2,354 | dtsort-em | 2155661.03 | 1402325.74 | **+53.7%** | ✅ |
| 2,644 | dtsort-am | 2483105.00 | 1600764.31 | **+55.1%** | ✅ |
| 2,644 | dtsort-em | 2483105.00 | 1615346.05 | **+53.7%** | ✅ |
| 2,970 | dtsort-am | 2915844.71 | 1888772.30 | **+54.4%** | ✅ |
| 2,970 | dtsort-em | 2915844.71 | 1907254.38 | **+52.9%** | ✅ |
| 3,336 | dtsort-am | 3337815.01 | 2256814.33 | **+47.9%** | ✅ |
| 3,336 | dtsort-em | 3337815.01 | 2256768.88 | **+47.9%** | ✅ |
| 3,748 | dtsort-am | 3941183.62 | 2654962.63 | **+48.4%** | ✅ |
| 3,748 | dtsort-em | 3941183.62 | 2660410.35 | **+48.1%** | ✅ |
| 4,210 | dtsort-am | 4594417.76 | 3134082.01 | **+46.6%** | ✅ |
| 4,210 | dtsort-em | 4594417.76 | 3133274.55 | **+46.6%** | ✅ |
| 4,729 | dtsort-am | 5132987.69 | 3726164.67 | **+37.8%** | ✅ |
| 4,729 | dtsort-em | 5132987.69 | 3718408.24 | **+38.0%** | ✅ |
| 5,312 | dtsort-am | 6003544.60 | 4819472.41 | **+24.6%** | ✅ |
| 5,312 | dtsort-em | 6003544.60 | 4790381.56 | **+25.3%** | ✅ |
| 5,967 | dtsort-am | 7033974.23 | 5923024.78 | **+18.8%** | ✅ |
| 5,967 | dtsort-em | 7033974.23 | 5936963.63 | **+18.5%** | ✅ |
| 6,703 | dtsort-am | 8149010.79 | 6822072.20 | **+19.5%** | ✅ |
| 6,703 | dtsort-em | 8149010.79 | 6858376.64 | **+18.8%** | ✅ |
| 7,530 | dtsort-am | 9453445.21 | 7880143.94 | **+20.0%** | ✅ |
| 7,530 | dtsort-em | 9453445.21 | 7993002.98 | **+18.3%** | ✅ |
| 8,458 | dtsort-am | 10877962.97 | 9008313.86 | **+20.8%** | ✅ |
| 8,458 | dtsort-em | 10877962.97 | 9118606.91 | **+19.3%** | ✅ |
| 9,502 | dtsort-am | 12620419.69 | 10423125.63 | **+21.1%** | ✅ |
| 9,502 | dtsort-em | 12620419.69 | 10451722.23 | **+20.7%** | ✅ |
| 10,673 | dtsort-am | 14586679.08 | 11936308.61 | **+22.2%** | ✅ |
| 10,673 | dtsort-em | 14586679.08 | 12013330.45 | **+21.4%** | ✅ |
| 11,990 | dtsort-am | 16839607.71 | 13668688.73 | **+23.2%** | ✅ |
| 11,990 | dtsort-em | 16839607.71 | 13720708.34 | **+22.7%** | ✅ |
| 13,468 | dtsort-am | 19472152.78 | 15689120.38 | **+24.1%** | ✅ |
| 13,468 | dtsort-em | 19472152.78 | 15731390.14 | **+23.8%** | ✅ |
| 15,129 | dtsort-am | 22668794.36 | 17977723.28 | **+26.1%** | ✅ |
| 15,129 | dtsort-em | 22668794.36 | 18085439.68 | **+25.3%** | ✅ |
| 16,995 | dtsort-am | 25897495.36 | 20650762.26 | **+25.4%** | ✅ |
| 16,995 | dtsort-em | 25897495.36 | 20603051.47 | **+25.7%** | ✅ |
| 19,091 | dtsort-am | 29372913.20 | 23456320.83 | **+25.2%** | ✅ |
| 19,091 | dtsort-em | 29372913.20 | 23453429.17 | **+25.2%** | ✅ |
| 21,445 | dtsort-am | 33722650.76 | 28940137.17 | **+16.5%** | ✅ |
| 21,445 | dtsort-em | 33722650.76 | 29054376.71 | **+16.1%** | ✅ |
| 24,090 | dtsort-am | 38466495.33 | 33778940.47 | **+13.9%** | ✅ |
| 24,090 | dtsort-em | 38466495.33 | 33847833.34 | **+13.6%** | ✅ |
| 27,061 | dtsort-am | 44171192.68 | 37993812.51 | **+16.3%** | ✅ |
| 27,061 | dtsort-em | 44171192.68 | 38368449.12 | **+15.1%** | ✅ |
| 30,398 | dtsort-am | 49926054.45 | 42785825.56 | **+16.7%** | ✅ |
| 30,398 | dtsort-em | 49926054.45 | 42948914.07 | **+16.2%** | ✅ |
| 34,147 | dtsort-am | 56519026.47 | 48155118.99 | **+17.4%** | ✅ |
| 34,147 | dtsort-em | 56519026.47 | 48435949.42 | **+16.7%** | ✅ |
| 38,358 | dtsort-am | 64580912.52 | 54184409.65 | **+19.2%** | ✅ |
| 38,358 | dtsort-em | 64580912.52 | 54451718.75 | **+18.6%** | ✅ |
| 43,089 | dtsort-am | 72572773.12 | 61500695.79 | **+18.0%** | ✅ |
| 43,089 | dtsort-em | 72572773.12 | 61822212.51 | **+17.4%** | ✅ |
| 48,403 | dtsort-am | 82918135.48 | 69868888.89 | **+18.7%** | ✅ |
| 48,403 | dtsort-em | 82918135.48 | 70159620.45 | **+18.2%** | ✅ |
| 54,372 | dtsort-am | 94568423.66 | 78692697.86 | **+20.2%** | ✅ |
| 54,372 | dtsort-em | 94568423.66 | 78895145.85 | **+19.9%** | ✅ |
| 61,077 | dtsort-am | 108600659.68 | 89988172.72 | **+20.7%** | ✅ |
| 61,077 | dtsort-em | 108600659.68 | 90041101.15 | **+20.6%** | ✅ |
| 68,609 | dtsort-am | 121968925.00 | 101668041.67 | **+20.0%** | ✅ |
| 68,609 | dtsort-em | 121968925.00 | 101797625.00 | **+19.8%** | ✅ |
| 77,071 | dtsort-am | 139384968.73 | 114604975.00 | **+21.6%** | ✅ |
| 77,071 | dtsort-em | 139384968.73 | 114666641.60 | **+21.6%** | ✅ |
| 86,575 | dtsort-am | 159302072.72 | 141056260.50 | **+12.9%** | ✅ |
| 86,575 | dtsort-em | 159302072.72 | 141218875.01 | **+12.8%** | ✅ |
| 97,252 | dtsort-am | 181426944.68 | 160539291.74 | **+13.0%** | ✅ |
| 97,252 | dtsort-em | 181426944.68 | 161042312.51 | **+12.7%** | ✅ |
| 109,246 | dtsort-am | 207496027.62 | 181678718.71 | **+14.2%** | ✅ |
| 109,246 | dtsort-em | 207496027.62 | 182977972.40 | **+13.4%** | ✅ |
| 122,718 | dtsort-am | 236324902.62 | 205744291.66 | **+14.9%** | ✅ |
| 122,718 | dtsort-em | 236324902.62 | 206588458.35 | **+14.4%** | ✅ |
| 137,852 | dtsort-am | 268508770.50 | 233566389.03 | **+15.0%** | ✅ |
| 137,852 | dtsort-em | 268508770.50 | 234528444.32 | **+14.5%** | ✅ |
| 154,853 | dtsort-am | 305157646.07 | 263952687.50 | **+15.6%** | ✅ |
| 154,853 | dtsort-em | 305157646.07 | 264935728.98 | **+15.2%** | ✅ |
| 173,950 | dtsort-am | 345138708.48 | 299376770.97 | **+15.3%** | ✅ |
| 173,950 | dtsort-em | 345138708.48 | 300542145.50 | **+14.8%** | ✅ |
| 195,402 | dtsort-am | 394978916.97 | 339626437.52 | **+16.3%** | ✅ |
| 195,402 | dtsort-em | 394978916.97 | 339189562.48 | **+16.4%** | ✅ |
| 219,500 | dtsort-am | 447831958.53 | 383951333.47 | **+16.6%** | ✅ |
| 219,500 | dtsort-em | 447831958.53 | 384360875.00 | **+16.5%** | ✅ |
| 246,569 | dtsort-am | 511065166.91 | 435760166.96 | **+17.3%** | ✅ |
| 246,569 | dtsort-em | 511065166.91 | 435937333.04 | **+17.2%** | ✅ |
| 276,977 | dtsort-am | 578677583.02 | 492438416.93 | **+17.5%** | ✅ |
| 276,977 | dtsort-em | 578677583.02 | 501069291.96 | **+15.5%** | ✅ |
| 311,135 | dtsort-am | 654266792.12 | 554055042.10 | **+18.1%** | ✅ |
| 311,135 | dtsort-em | 654266792.12 | 555550500.05 | **+17.8%** | ✅ |
| 349,506 | dtsort-am | 751676749.90 | 676240083.06 | **+11.2%** | ✅ |
| 349,506 | dtsort-em | 751676749.90 | 678681791.99 | **+10.8%** | ✅ |
| 392,608 | dtsort-am | 857219917.00 | 761349999.81 | **+12.6%** | ✅ |
| 392,608 | dtsort-em | 857219917.00 | 760330417.19 | **+12.7%** | ✅ |
| 441,026 | dtsort-am | 975110332.95 | 870289416.98 | **+12.0%** | ✅ |
| 441,026 | dtsort-em | 975110332.95 | 867158500.01 | **+12.4%** | ✅ |
| 495,415 | dtsort-am | 1110950790.81 | 980560458.96 | **+13.3%** | ✅ |
| 495,415 | dtsort-em | 1110950790.81 | 984670790.84 | **+12.8%** | ✅ |
| 556,512 | dtsort-am | 1245440957.83 | 1102539792.19 | **+13.0%** | ✅ |
| 556,512 | dtsort-em | 1245440957.83 | 1114561709.11 | **+11.7%** | ✅ |
| 625,143 | dtsort-am | 1415937375.04 | 1248233708.09 | **+13.4%** | ✅ |
| 625,143 | dtsort-em | 1415937375.04 | 1252316625.09 | **+13.1%** | ✅ |
| 702,238 | dtsort-am | 1625030457.97 | 1421945209.15 | **+14.3%** | ✅ |
| 702,238 | dtsort-em | 1625030457.97 | 1419163665.97 | **+14.5%** | ✅ |
| 788,841 | dtsort-am | 1821646917.15 | 1594524832.91 | **+14.2%** | ✅ |
| 788,841 | dtsort-em | 1821646917.15 | 1601427250.08 | **+13.8%** | ✅ |
| 886,124 | dtsort-am | 2076350250.05 | 1810292500.07 | **+14.7%** | ✅ |
| 886,124 | dtsort-em | 2076350250.05 | 1813165457.92 | **+14.5%** | ✅ |
| 995,405 | dtsort-am | 2357644832.93 | 2060930707.96 | **+14.4%** | ✅ |
| 995,405 | dtsort-em | 2357644832.93 | 2063021666.84 | **+14.3%** | ✅ |
| 1,118,162 | dtsort-am | 2917819666.91 | 2538337999.95 | **+15.0%** | ✅ |
| 1,118,162 | dtsort-em | 2917819666.91 | 2552567416.10 | **+14.3%** | ✅ |
| 1,256,058 | dtsort-am | 3334029832.85 | 2879240792.01 | **+15.8%** | ✅ |
| 1,256,058 | dtsort-em | 3334029832.85 | 2895854041.91 | **+15.1%** | ✅ |
| 1,410,960 | dtsort-am | 3774265290.94 | 3467443166.07 | **+8.8%** | ✅ |
| 1,410,960 | dtsort-em | 3774265290.94 | 3472940916.89 | **+8.7%** | ✅ |
| 1,584,966 | dtsort-am | 4313846833.07 | 3911712083.97 | **+10.3%** | ✅ |
| 1,584,966 | dtsort-em | 4313846833.07 | 3905677624.98 | **+10.5%** | ✅ |
| 1,780,430 | dtsort-am | 5003558124.88 | 4414050541.12 | **+13.4%** | ✅ |
| 1,780,430 | dtsort-em | 5003558124.88 | 4419071583.08 | **+13.2%** | ✅ |
| 2,000,000 | dtsort-am | 5537236707.98 | 4972068709.09 | **+11.4%** | ✅ |
| 2,000,000 | dtsort-em | 5537236707.98 | 4986328291.94 | **+11.0%** | ✅ |

#### dt (WideRow (Sorted))
| Input Size | Variant | Stock ns | dtsort ns | Gain | Status |
|------------|---------|----------|-----------|------|--------|
| 2 | dtsort-am | 104.16 | 84.11 | **+23.8%** | ✅ |
| 2 | dtsort-em | 104.16 | 84.63 | **+23.1%** | ✅ |
| 3 | dtsort-am | 369.03 | 120.85 | **+205.4%** | ✅ |
| 3 | dtsort-em | 369.03 | 123.59 | **+198.6%** | ✅ |
| 4 | dtsort-am | 485.58 | 168.58 | **+188.0%** | ✅ |
| 4 | dtsort-em | 485.58 | 174.42 | **+178.4%** | ✅ |
| 5 | dtsort-am | 627.98 | 211.17 | **+197.4%** | ✅ |
| 5 | dtsort-em | 627.98 | 232.49 | **+170.1%** | ✅ |
| 6 | dtsort-am | 747.50 | 259.32 | **+188.3%** | ✅ |
| 6 | dtsort-em | 747.50 | 285.72 | **+161.6%** | ✅ |
| 7 | dtsort-am | 860.24 | 310.88 | **+176.7%** | ✅ |
| 7 | dtsort-em | 860.24 | 328.95 | **+161.5%** | ✅ |
| 8 | dtsort-am | 1058.32 | 386.32 | **+173.9%** | ✅ |
| 8 | dtsort-em | 1058.32 | 383.42 | **+176.0%** | ✅ |
| 9 | dtsort-am | 1315.49 | 424.93 | **+209.6%** | ✅ |
| 9 | dtsort-em | 1315.49 | 421.85 | **+211.8%** | ✅ |
| 10 | dtsort-am | 1645.11 | 471.58 | **+248.9%** | ✅ |
| 10 | dtsort-em | 1645.11 | 467.77 | **+251.7%** | ✅ |
| 20 | dtsort-am | 6881.20 | 5618.41 | **+22.5%** | ✅ |
| 20 | dtsort-em | 6881.20 | 5648.55 | **+21.8%** | ✅ |
| 22 | dtsort-am | 7764.63 | 6119.48 | **+26.9%** | ✅ |
| 22 | dtsort-em | 7764.63 | 6079.30 | **+27.7%** | ✅ |
| 25 | dtsort-am | 8871.94 | 6829.21 | **+29.9%** | ✅ |
| 25 | dtsort-em | 8871.94 | 6882.52 | **+28.9%** | ✅ |
| 28 | dtsort-am | 10038.22 | 7556.90 | **+32.8%** | ✅ |
| 28 | dtsort-em | 10038.22 | 7407.25 | **+35.5%** | ✅ |
| 32 | dtsort-am | 11623.69 | 8437.78 | **+37.8%** | ✅ |
| 32 | dtsort-em | 11623.69 | 8577.57 | **+35.5%** | ✅ |
| 36 | dtsort-am | 13091.55 | 9462.58 | **+38.4%** | ✅ |
| 36 | dtsort-em | 13091.55 | 9432.50 | **+38.8%** | ✅ |
| 40 | dtsort-am | 14922.40 | 10379.76 | **+43.8%** | ✅ |
| 40 | dtsort-em | 14922.40 | 10414.81 | **+43.3%** | ✅ |
| 45 | dtsort-am | 16803.13 | 11749.77 | **+43.0%** | ✅ |
| 45 | dtsort-em | 16803.13 | 11801.07 | **+42.4%** | ✅ |
| 51 | dtsort-am | 19223.20 | 13148.58 | **+46.2%** | ✅ |
| 51 | dtsort-em | 19223.20 | 13151.25 | **+46.2%** | ✅ |
| 57 | dtsort-am | 21430.70 | 14590.33 | **+46.9%** | ✅ |
| 57 | dtsort-em | 21430.70 | 14581.85 | **+47.0%** | ✅ |
| 64 | dtsort-am | 24125.84 | 16245.74 | **+48.5%** | ✅ |
| 64 | dtsort-em | 24125.84 | 16476.69 | **+46.4%** | ✅ |
| 72 | dtsort-am | 34563.34 | 18138.94 | **+90.5%** | ✅ |
| 72 | dtsort-em | 34563.34 | 18350.28 | **+88.4%** | ✅ |
| 81 | dtsort-am | 39730.41 | 20431.56 | **+94.5%** | ✅ |
| 81 | dtsort-em | 39730.41 | 20514.21 | **+93.7%** | ✅ |
| 91 | dtsort-am | 45261.96 | 22738.30 | **+99.1%** | ✅ |
| 91 | dtsort-em | 45261.96 | 23386.23 | **+93.5%** | ✅ |
| 102 | dtsort-am | 50752.93 | 25336.54 | **+100.3%** | ✅ |
| 102 | dtsort-em | 50752.93 | 26022.52 | **+95.0%** | ✅ |
| 114 | dtsort-am | 57669.78 | 28515.03 | **+102.2%** | ✅ |
| 114 | dtsort-em | 57669.78 | 28808.70 | **+100.2%** | ✅ |
| 129 | dtsort-am | 65402.93 | 32066.65 | **+104.0%** | ✅ |
| 129 | dtsort-em | 65402.93 | 32305.82 | **+102.4%** | ✅ |
| 144 | dtsort-am | 73845.00 | 35635.61 | **+107.2%** | ✅ |
| 144 | dtsort-em | 73845.00 | 36407.27 | **+102.8%** | ✅ |
| 162 | dtsort-am | 84033.00 | 40002.88 | **+110.1%** | ✅ |
| 162 | dtsort-em | 84033.00 | 40732.72 | **+106.3%** | ✅ |
| 182 | dtsort-am | 94977.65 | 44629.50 | **+112.8%** | ✅ |
| 182 | dtsort-em | 94977.65 | 45711.97 | **+107.8%** | ✅ |
| 205 | dtsort-am | 106708.84 | 50832.96 | **+109.9%** | ✅ |
| 205 | dtsort-em | 106708.84 | 51242.66 | **+108.2%** | ✅ |
| 230 | dtsort-am | 120652.72 | 56873.15 | **+112.1%** | ✅ |
| 230 | dtsort-em | 120652.72 | 58026.97 | **+107.9%** | ✅ |
| 258 | dtsort-am | 137446.16 | 63430.20 | **+116.7%** | ✅ |
| 258 | dtsort-em | 137446.16 | 65028.47 | **+111.4%** | ✅ |
| 290 | dtsort-am | 182917.43 | 71427.81 | **+156.1%** | ✅ |
| 290 | dtsort-em | 182917.43 | 72923.17 | **+150.8%** | ✅ |
| 326 | dtsort-am | 208652.97 | 105627.80 | **+97.5%** | ✅ |
| 326 | dtsort-em | 208652.97 | 107772.50 | **+93.6%** | ✅ |
| 366 | dtsort-am | 235865.89 | 144983.13 | **+62.7%** | ✅ |
| 366 | dtsort-em | 235865.89 | 147972.88 | **+59.4%** | ✅ |
| 411 | dtsort-am | 269325.06 | 164194.91 | **+64.0%** | ✅ |
| 411 | dtsort-em | 269325.06 | 166569.67 | **+61.7%** | ✅ |
| 462 | dtsort-am | 303541.27 | 184239.41 | **+64.8%** | ✅ |
| 462 | dtsort-em | 303541.27 | 186228.32 | **+63.0%** | ✅ |
| 519 | dtsort-am | 343276.96 | 207285.18 | **+65.6%** | ✅ |
| 519 | dtsort-em | 343276.96 | 209723.21 | **+63.7%** | ✅ |
| 583 | dtsort-am | 390810.79 | 231544.92 | **+68.8%** | ✅ |
| 583 | dtsort-em | 390810.79 | 235941.94 | **+65.6%** | ✅ |
| 655 | dtsort-am | 442822.82 | 260571.03 | **+69.9%** | ✅ |
| 655 | dtsort-em | 442822.82 | 264866.75 | **+67.2%** | ✅ |
| 736 | dtsort-am | 497571.01 | 297963.96 | **+67.0%** | ✅ |
| 736 | dtsort-em | 497571.01 | 297048.14 | **+67.5%** | ✅ |
| 826 | dtsort-am | 558990.99 | 329941.10 | **+69.4%** | ✅ |
| 826 | dtsort-em | 558990.99 | 333335.94 | **+67.7%** | ✅ |
| 928 | dtsort-am | 629377.36 | 380151.13 | **+65.6%** | ✅ |
| 928 | dtsort-em | 629377.36 | 375837.66 | **+67.5%** | ✅ |
| 1,043 | dtsort-am | 731185.16 | 421859.41 | **+73.3%** | ✅ |
| 1,043 | dtsort-em | 731185.16 | 435498.26 | **+67.9%** | ✅ |
| 1,171 | dtsort-am | 928254.63 | 476715.18 | **+94.7%** | ✅ |
| 1,171 | dtsort-em | 928254.63 | 489609.98 | **+89.6%** | ✅ |
| 1,316 | dtsort-am | 1052711.58 | 618293.90 | **+70.3%** | ✅ |
| 1,316 | dtsort-em | 1052711.58 | 629218.70 | **+67.3%** | ✅ |
| 1,478 | dtsort-am | 1196494.52 | 849836.21 | **+40.8%** | ✅ |
| 1,478 | dtsort-em | 1196494.52 | 841977.69 | **+42.1%** | ✅ |
| 1,660 | dtsort-am | 1359756.30 | 948745.29 | **+43.3%** | ✅ |
| 1,660 | dtsort-em | 1359756.30 | 951183.11 | **+43.0%** | ✅ |
| 1,865 | dtsort-am | 1530926.39 | 1068150.17 | **+43.3%** | ✅ |
| 1,865 | dtsort-em | 1530926.39 | 1085045.07 | **+41.1%** | ✅ |
| 2,095 | dtsort-am | 1747863.90 | 1217486.50 | **+43.6%** | ✅ |
| 2,095 | dtsort-em | 1747863.90 | 1231681.90 | **+41.9%** | ✅ |
| 2,354 | dtsort-am | 1990213.35 | 1404808.78 | **+41.7%** | ✅ |
| 2,354 | dtsort-em | 1990213.35 | 1421059.69 | **+40.1%** | ✅ |
| 2,644 | dtsort-am | 2270305.96 | 1633656.03 | **+39.0%** | ✅ |
| 2,644 | dtsort-em | 2270305.96 | 1635766.23 | **+38.8%** | ✅ |
| 2,970 | dtsort-am | 2614458.65 | 1925151.46 | **+35.8%** | ✅ |
| 2,970 | dtsort-em | 2614458.65 | 1945755.03 | **+34.4%** | ✅ |
| 3,336 | dtsort-am | 3014470.55 | 2283503.15 | **+32.0%** | ✅ |
| 3,336 | dtsort-em | 3014470.55 | 2276772.01 | **+32.4%** | ✅ |
| 3,748 | dtsort-am | 3476481.69 | 2653571.55 | **+31.0%** | ✅ |
| 3,748 | dtsort-em | 3476481.69 | 2666436.29 | **+30.4%** | ✅ |
| 4,210 | dtsort-am | 4144869.79 | 3108096.54 | **+33.4%** | ✅ |
| 4,210 | dtsort-em | 4144869.79 | 3109053.12 | **+33.3%** | ✅ |
| 4,729 | dtsort-am | 5011431.48 | 3636536.24 | **+37.8%** | ✅ |
| 4,729 | dtsort-em | 5011431.48 | 3626468.53 | **+38.2%** | ✅ |
| 5,312 | dtsort-am | 6008523.35 | 4759868.43 | **+26.2%** | ✅ |
| 5,312 | dtsort-em | 6008523.35 | 4766469.46 | **+26.1%** | ✅ |
| 5,967 | dtsort-am | 6975408.83 | 5848729.87 | **+19.3%** | ✅ |
| 5,967 | dtsort-em | 6975408.83 | 5850969.64 | **+19.2%** | ✅ |
| 6,703 | dtsort-am | 8063041.67 | 6707241.91 | **+20.2%** | ✅ |
| 6,703 | dtsort-em | 8063041.67 | 6726232.20 | **+19.9%** | ✅ |
| 7,530 | dtsort-am | 9275878.29 | 7759987.83 | **+19.5%** | ✅ |
| 7,530 | dtsort-em | 9275878.29 | 7770677.90 | **+19.4%** | ✅ |
| 8,458 | dtsort-am | 10617187.19 | 8909180.74 | **+19.2%** | ✅ |
| 8,458 | dtsort-em | 10617187.19 | 8918923.16 | **+19.0%** | ✅ |
| 9,502 | dtsort-am | 12176555.56 | 10197858.21 | **+19.4%** | ✅ |
| 9,502 | dtsort-em | 12176555.56 | 10249032.47 | **+18.8%** | ✅ |
| 10,673 | dtsort-am | 13942979.59 | 11624110.42 | **+19.9%** | ✅ |
| 10,673 | dtsort-em | 13942979.59 | 11694887.50 | **+19.2%** | ✅ |
| 11,990 | dtsort-am | 15981543.55 | 13340387.02 | **+19.8%** | ✅ |
| 11,990 | dtsort-em | 15981543.55 | 13364207.52 | **+19.6%** | ✅ |
| 13,468 | dtsort-am | 18315804.82 | 15529488.22 | **+17.9%** | ✅ |
| 13,468 | dtsort-em | 18315804.82 | 15348100.54 | **+19.3%** | ✅ |
| 15,129 | dtsort-am | 20927201.00 | 17641151.72 | **+18.6%** | ✅ |
| 15,129 | dtsort-em | 20927201.00 | 17534692.73 | **+19.3%** | ✅ |
| 16,995 | dtsort-am | 24263964.07 | 20110225.00 | **+20.7%** | ✅ |
| 16,995 | dtsort-em | 24263964.07 | 20040964.29 | **+21.1%** | ✅ |
| 19,091 | dtsort-am | 28634279.50 | 22799275.55 | **+25.6%** | ✅ |
| 19,091 | dtsort-em | 28634279.50 | 22774611.55 | **+25.7%** | ✅ |
| 21,445 | dtsort-am | 32784138.91 | 28302460.00 | **+15.8%** | ✅ |
| 21,445 | dtsort-em | 32784138.91 | 28536650.00 | **+14.9%** | ✅ |
| 24,090 | dtsort-am | 37555947.36 | 32921585.29 | **+14.1%** | ✅ |
| 24,090 | dtsort-em | 37555947.36 | 33006644.86 | **+13.8%** | ✅ |
| 27,061 | dtsort-am | 42448695.31 | 37250223.68 | **+14.0%** | ✅ |
| 27,061 | dtsort-em | 42448695.31 | 37228782.89 | **+14.0%** | ✅ |
| 30,398 | dtsort-am | 48065014.85 | 41958960.83 | **+14.6%** | ✅ |
| 30,398 | dtsort-em | 48065014.85 | 41964007.35 | **+14.5%** | ✅ |
| 34,147 | dtsort-am | 54386909.76 | 47289491.07 | **+15.0%** | ✅ |
| 34,147 | dtsort-em | 54386909.76 | 47357767.85 | **+14.8%** | ✅ |
| 38,358 | dtsort-am | 61473227.28 | 53292680.49 | **+15.4%** | ✅ |
| 38,358 | dtsort-em | 61473227.28 | 53409468.75 | **+15.1%** | ✅ |
| 43,089 | dtsort-am | 69581532.46 | 60195261.36 | **+15.6%** | ✅ |
| 43,089 | dtsort-em | 69581532.46 | 60245983.40 | **+15.5%** | ✅ |
| 48,403 | dtsort-am | 78754348.88 | 67889562.51 | **+16.0%** | ✅ |
| 48,403 | dtsort-em | 78754348.88 | 68029963.00 | **+15.8%** | ✅ |
| 54,372 | dtsort-am | 89172160.72 | 77176489.63 | **+15.5%** | ✅ |
| 54,372 | dtsort-em | 89172160.72 | 76872416.63 | **+16.0%** | ✅ |
| 61,077 | dtsort-am | 100975208.33 | 87393381.00 | **+15.5%** | ✅ |
| 61,077 | dtsort-em | 100975208.33 | 87095404.71 | **+15.9%** | ✅ |
| 68,609 | dtsort-am | 116729433.37 | 99133361.17 | **+17.7%** | ✅ |
| 68,609 | dtsort-em | 116729433.37 | 98539118.00 | **+18.5%** | ✅ |
| 77,071 | dtsort-am | 136307808.38 | 111983062.51 | **+21.7%** | ✅ |
| 77,071 | dtsort-em | 136307808.38 | 112029354.17 | **+21.7%** | ✅ |
| 86,575 | dtsort-am | 156281520.73 | 138762466.61 | **+12.6%** | ✅ |
| 86,575 | dtsort-em | 156281520.73 | 139032520.99 | **+12.4%** | ✅ |
| 97,252 | dtsort-am | 177418896.00 | 158526812.51 | **+11.9%** | ✅ |
| 97,252 | dtsort-em | 177418896.00 | 158444812.52 | **+12.0%** | ✅ |
| 109,246 | dtsort-am | 200739653.00 | 179806603.99 | **+11.6%** | ✅ |
| 109,246 | dtsort-em | 200739653.00 | 179359736.00 | **+11.9%** | ✅ |
| 122,718 | dtsort-am | 226619791.71 | 202763305.67 | **+11.8%** | ✅ |
| 122,718 | dtsort-em | 226619791.71 | 202679847.29 | **+11.8%** | ✅ |
| 137,852 | dtsort-am | 257252166.98 | 229816388.96 | **+11.9%** | ✅ |
| 137,852 | dtsort-em | 257252166.98 | 229930541.63 | **+11.9%** | ✅ |
| 154,853 | dtsort-am | 291729812.51 | 261355569.35 | **+11.6%** | ✅ |
| 154,853 | dtsort-em | 291729812.51 | 259822041.49 | **+12.3%** | ✅ |
| 173,950 | dtsort-am | 330196895.99 | 293486771.06 | **+12.5%** | ✅ |
| 173,950 | dtsort-em | 330196895.99 | 292766479.43 | **+12.8%** | ✅ |
| 195,402 | dtsort-am | 373489020.97 | 332134666.50 | **+12.5%** | ✅ |
| 195,402 | dtsort-em | 373489020.97 | 332209312.47 | **+12.4%** | ✅ |
| 219,500 | dtsort-am | 422667416.98 | 375712395.99 | **+12.5%** | ✅ |
| 219,500 | dtsort-em | 422667416.98 | 374376958.59 | **+12.9%** | ✅ |
| 246,569 | dtsort-am | 483900625.03 | 425554228.95 | **+13.7%** | ✅ |
| 246,569 | dtsort-em | 483900625.03 | 425011479.06 | **+13.9%** | ✅ |
| 276,977 | dtsort-am | 552084291.12 | 481134208.85 | **+14.7%** | ✅ |
| 276,977 | dtsort-em | 552084291.12 | 479216666.89 | **+15.2%** | ✅ |
| 311,135 | dtsort-am | 641635166.95 | 544174415.99 | **+17.9%** | ✅ |
| 311,135 | dtsort-em | 641635166.95 | 542142832.88 | **+18.4%** | ✅ |
| 349,506 | dtsort-am | 729771542.13 | 666300083.98 | **+9.5%** | ✅ |
| 349,506 | dtsort-em | 729771542.13 | 666828874.96 | **+9.4%** | ✅ |
| 392,608 | dtsort-am | 827210332.97 | 754762500.06 | **+9.6%** | ✅ |
| 392,608 | dtsort-em | 827210332.97 | 748713959.01 | **+10.5%** | ✅ |
| 441,026 | dtsort-am | 937659749.99 | 853405707.98 | **+9.9%** | ✅ |
| 441,026 | dtsort-em | 937659749.99 | 849978875.14 | **+10.3%** | ✅ |
| 495,415 | dtsort-am | 1061367792.08 | 968418583.04 | **+9.6%** | ✅ |
| 495,415 | dtsort-em | 1061367792.08 | 969812333.12 | **+9.4%** | ✅ |
| 556,512 | dtsort-am | 1201639250.15 | 1087653666.97 | **+10.5%** | ✅ |
| 556,512 | dtsort-em | 1201639250.15 | 1086262749.97 | **+10.6%** | ✅ |
| 625,143 | dtsort-am | 1352029290.98 | 1230885915.92 | **+9.8%** | ✅ |
| 625,143 | dtsort-em | 1352029290.98 | 1226992417.13 | **+10.2%** | ✅ |
| 702,238 | dtsort-am | 1539207666.88 | 1385815792.02 | **+11.1%** | ✅ |
| 702,238 | dtsort-em | 1539207666.88 | 1383208832.94 | **+11.3%** | ✅ |
| 788,841 | dtsort-am | 1729368167.00 | 1567254458.09 | **+10.3%** | ✅ |
| 788,841 | dtsort-em | 1729368167.00 | 1560362666.15 | **+10.8%** | ✅ |
| 886,124 | dtsort-am | 1963003000.01 | 1773190957.96 | **+10.7%** | ✅ |
| 886,124 | dtsort-em | 1963003000.01 | 1765334625.03 | **+11.2%** | ✅ |
| 995,405 | dtsort-am | 2221304750.07 | 2002842457.97 | **+10.9%** | ✅ |
| 995,405 | dtsort-em | 2221304750.07 | 1998090500.01 | **+11.2%** | ✅ |
| 1,118,162 | dtsort-am | 2789844000.01 | 2453428125.01 | **+13.7%** | ✅ |
| 1,118,162 | dtsort-em | 2789844000.01 | 2516384166.90 | **+10.9%** | ✅ |
| 1,256,058 | dtsort-am | 3247869750.02 | 2826350042.13 | **+14.9%** | ✅ |
| 1,256,058 | dtsort-em | 3247869750.02 | 2826208458.05 | **+14.9%** | ✅ |
| 1,410,960 | dtsort-am | 3703691083.94 | 3408280250.15 | **+8.7%** | ✅ |
| 1,410,960 | dtsort-em | 3703691083.94 | 3412763791.16 | **+8.5%** | ✅ |
| 1,584,966 | dtsort-am | 4194586917.06 | 3853470166.91 | **+8.9%** | ✅ |
| 1,584,966 | dtsort-em | 4194586917.06 | 3836051999.82 | **+9.3%** | ✅ |
| 1,780,430 | dtsort-am | 4744267042.03 | 4350271791.92 | **+9.1%** | ✅ |
| 1,780,430 | dtsort-em | 4744267042.03 | 4339969832.92 | **+9.3%** | ✅ |
| 2,000,000 | dtsort-am | 5376355957.94 | 4913972374.98 | **+9.4%** | ✅ |
| 2,000,000 | dtsort-em | 5376355957.94 | 4899896624.96 | **+9.7%** | ✅ |

### 4.2 Data Type Weights

*Source: `v_weight_summary` view*

| Data Type | Cmp (ns) | Cmp Factor | Move (ns) | Move Factor |
|-----------|----------|------------|-----------|-------------|
| IntNaked | 0.35 | 1.0x | 0.65 | 1.0x |
| WideRow | 7.13 | 20.1x | 187.88 | 287.2x |

## 5. Adaptivity Comparison (dt_stable_sort)

Comparing the two main configuration modes: High Entropy (No early exits or merge-skipping) vs. Adaptive Mode (GS shortcuts + merge-skipping).

*Source: `wall_clock` table*

#### IntNaked (Random)
| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |
|------|-------------------|-------------------|------|
| 2 | 3.22 | 4.45 | -27.7% |
| 3 | 4.10 | 5.75 | -28.8% |
| 4 | 4.70 | 6.85 | -31.4% |
| 5 | 5.00 | 6.86 | -27.1% |
| 6 | 6.94 | 7.63 | -9.1% |
| 7 | 7.89 | 10.37 | -23.9% |
| 8 | 8.75 | 11.12 | -21.2% |
| 9 | 9.18 | 13.13 | -30.1% |
| 10 | 10.93 | 13.86 | -21.1% |
| 20 | 55.75 | 56.79 | -1.8% |
| 22 | 73.57 | 78.21 | -5.9% |
| 25 | 88.45 | 87.34 | **+1.3%** |
| 28 | 98.81 | 95.70 | **+3.2%** |
| 32 | 122.51 | 123.65 | -0.9% |
| 36 | 152.01 | 148.30 | **+2.5%** |
| 40 | 149.21 | 153.04 | -2.5% |
| 45 | 192.60 | 194.44 | -0.9% |
| 51 | 228.67 | 227.13 | **+0.7%** |
| 57 | 270.81 | 269.20 | **+0.6%** |
| 64 | 327.76 | 326.15 | +0.5% |
| 72 | 376.25 | 373.00 | **+0.9%** |
| 81 | 454.49 | 448.17 | **+1.4%** |
| 91 | 536.99 | 534.76 | +0.4% |
| 102 | 619.40 | 620.71 | -0.2% |
| 114 | 726.31 | 717.70 | **+1.2%** |
| 129 | 859.03 | 860.21 | -0.1% |
| 144 | 964.29 | 967.76 | -0.4% |
| 162 | 1088.08 | 1093.95 | -0.5% |
| 182 | 1305.72 | 1303.03 | +0.2% |
| 205 | 1547.17 | 1546.81 | +0.0% |
| 230 | 1804.40 | 1820.32 | -0.9% |
| 258 | 2150.36 | 2165.15 | -0.7% |
| 290 | 2488.82 | 2490.90 | -0.1% |
| 326 | 2926.00 | 2874.60 | **+1.8%** |
| 366 | 3399.58 | 3358.35 | **+1.2%** |
| 411 | 3842.27 | 3873.14 | -0.8% |
| 462 | 4461.16 | 4478.28 | -0.4% |
| 519 | 5169.57 | 5229.95 | -1.2% |
| 583 | 5914.92 | 6023.78 | -1.8% |
| 655 | 6810.63 | 6962.83 | -2.2% |
| 736 | 7951.08 | 8120.84 | -2.1% |
| 826 | 9198.91 | 9432.54 | -2.5% |
| 928 | 10610.98 | 10765.24 | -1.4% |
| 1,043 | 12379.90 | 12544.67 | -1.3% |
| 1,171 | 14361.21 | 14374.45 | -0.1% |
| 1,316 | 16363.11 | 16436.56 | -0.4% |
| 1,478 | 18871.90 | 19063.17 | -1.0% |
| 1,660 | 21688.10 | 21906.71 | -1.0% |
| 1,865 | 24764.35 | 25229.43 | -1.8% |
| 2,095 | 28540.11 | 29255.58 | -2.4% |
| 2,354 | 32829.61 | 33606.69 | -2.3% |
| 2,644 | 37398.20 | 38296.10 | -2.3% |
| 2,970 | 43242.81 | 44052.01 | -1.8% |
| 3,336 | 49867.09 | 50578.01 | -1.4% |
| 3,748 | 57562.65 | 58157.33 | -1.0% |
| 4,210 | 66036.20 | 66716.41 | -1.0% |
| 4,729 | 75849.70 | 76180.18 | -0.4% |
| 5,312 | 86783.89 | 87010.06 | -0.3% |
| 5,967 | 99221.23 | 100098.05 | -0.9% |
| 6,703 | 113381.75 | 115362.02 | -1.7% |
| 7,530 | 129743.67 | 132381.34 | -2.0% |
| 8,458 | 149487.57 | 151919.78 | -1.6% |
| 9,502 | 169915.96 | 174226.40 | -2.5% |
| 10,673 | 193429.32 | 198426.19 | -2.5% |
| 11,990 | 222148.30 | 226848.42 | -2.1% |
| 13,468 | 255856.35 | 259785.57 | -1.5% |
| 15,129 | 293522.35 | 297598.78 | -1.4% |
| 16,995 | 334831.45 | 336258.46 | -0.4% |
| 19,091 | 379548.08 | 379896.48 | -0.1% |
| 21,445 | 429791.48 | 430654.17 | -0.2% |
| 24,090 | 492796.83 | 498471.40 | -1.1% |
| 27,061 | 562993.18 | 572484.81 | -1.7% |
| 30,398 | 643963.99 | 656703.86 | -1.9% |
| 34,147 | 732643.55 | 749262.40 | -2.2% |
| 38,358 | 830444.17 | 852887.42 | -2.6% |
| 43,089 | 945927.43 | 978941.42 | -3.4% |
| 48,403 | 1095837.87 | 1121376.29 | -2.3% |
| 54,372 | 1274519.71 | 1292453.32 | -1.4% |
| 61,077 | 1467532.52 | 1479991.63 | -0.8% |
| 68,609 | 1726492.13 | 1719578.54 | +0.4% |
| 77,071 | 1968835.13 | 1954029.61 | **+0.8%** |
| 86,575 | 2261301.78 | 2258891.07 | +0.1% |
| 97,252 | 2591483.21 | 2615880.21 | -0.9% |
| 109,246 | 2979658.59 | 3006102.17 | -0.9% |
| 122,718 | 3395505.72 | 3427945.56 | -0.9% |
| 137,852 | 3926273.74 | 3966006.47 | -1.0% |
| 154,853 | 4459086.87 | 4523321.75 | -1.4% |
| 173,950 | 5071522.53 | 5152022.08 | -1.6% |
| 195,402 | 5844636.75 | 5892502.84 | -0.8% |
| 219,500 | 6690343.04 | 6718921.92 | -0.4% |
| 246,569 | 7632981.68 | 7627515.56 | +0.1% |
| 276,977 | 8752908.23 | 8765237.34 | -0.1% |
| 311,135 | 9935022.63 | 9956080.96 | -0.2% |
| 349,506 | 11275838.04 | 11303190.19 | -0.2% |
| 392,608 | 12863863.43 | 12902395.05 | -0.3% |
| 441,026 | 14453033.00 | 14516229.17 | -0.4% |
| 495,415 | 16433535.86 | 16532247.03 | -0.6% |
| 556,512 | 18606572.36 | 18697898.65 | -0.5% |
| 625,143 | 20897051.76 | 21037045.45 | -0.7% |
| 702,238 | 23715216.97 | 23889843.37 | -0.7% |
| 788,841 | 27103982.35 | 27254386.20 | -0.6% |
| 886,124 | 30787815.22 | 30951197.48 | -0.5% |
| 995,405 | 34890729.15 | 35044091.71 | -0.4% |
| 1,118,162 | 39775356.50 | 39797650.44 | -0.1% |
| 1,256,058 | 44988346.30 | 44932263.00 | +0.1% |
| 1,410,960 | 50944583.36 | 50895348.21 | +0.1% |
| 1,584,966 | 57759093.74 | 57811468.74 | -0.1% |
| 1,780,430 | 65193545.45 | 65446856.09 | -0.4% |
| 2,000,000 | 73501555.56 | 73994504.55 | -0.7% |

#### IntNaked (Reverse)
| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |
|------|-------------------|-------------------|------|
| 2 | 3.22 | 4.42 | -27.2% |
| 3 | 4.09 | 5.68 | -28.0% |
| 4 | 4.70 | 6.99 | -32.7% |
| 5 | 4.97 | 6.92 | -28.1% |
| 6 | 6.05 | 7.49 | -19.3% |
| 7 | 7.53 | 8.81 | -14.5% |
| 8 | 9.85 | 12.29 | -19.8% |
| 9 | 12.80 | 11.72 | **+9.2%** |
| 10 | 11.72 | 11.52 | **+1.7%** |
| 20 | 47.04 | 45.98 | **+2.3%** |
| 22 | 54.28 | 55.19 | -1.7% |
| 25 | 69.91 | 67.71 | **+3.3%** |
| 28 | 83.57 | 78.21 | **+6.9%** |
| 32 | 101.28 | 101.80 | -0.5% |
| 36 | 125.11 | 120.45 | **+3.9%** |
| 40 | 104.74 | 100.40 | **+4.3%** |
| 45 | 146.16 | 141.52 | **+3.3%** |
| 51 | 176.00 | 169.41 | **+3.9%** |
| 57 | 219.41 | 218.54 | +0.4% |
| 64 | 238.57 | 235.33 | **+1.4%** |
| 72 | 276.72 | 269.59 | **+2.6%** |
| 81 | 291.40 | 291.13 | +0.1% |
| 91 | 369.70 | 372.73 | -0.8% |
| 102 | 436.01 | 433.42 | **+0.6%** |
| 114 | 507.05 | 499.92 | **+1.4%** |
| 129 | 584.95 | 581.15 | **+0.7%** |
| 144 | 626.73 | 622.06 | **+0.8%** |
| 162 | 678.57 | 674.61 | **+0.6%** |
| 182 | 843.95 | 832.44 | **+1.4%** |
| 205 | 1008.30 | 997.62 | **+1.1%** |
| 230 | 1160.19 | 1149.40 | **+0.9%** |
| 258 | 1366.56 | 1358.81 | **+0.6%** |
| 290 | 1513.26 | 1504.10 | **+0.6%** |
| 326 | 1728.91 | 1699.14 | **+1.8%** |
| 366 | 2093.15 | 2032.16 | **+3.0%** |
| 411 | 2393.90 | 2389.55 | +0.2% |
| 462 | 2725.28 | 2727.48 | -0.1% |
| 519 | 3144.09 | 3135.42 | +0.3% |
| 583 | 3473.24 | 3483.65 | -0.3% |
| 655 | 3839.81 | 3850.37 | -0.3% |
| 736 | 4741.90 | 4757.24 | -0.3% |
| 826 | 5500.85 | 5551.26 | -0.9% |
| 928 | 6317.56 | 6359.48 | -0.7% |
| 1,043 | 7173.37 | 7161.45 | +0.2% |
| 1,171 | 8124.78 | 8087.31 | +0.5% |
| 1,316 | 9109.13 | 9093.82 | +0.2% |
| 1,478 | 10875.63 | 10857.57 | +0.2% |
| 1,660 | 12479.40 | 12496.02 | -0.1% |
| 1,865 | 14248.81 | 14333.32 | -0.6% |
| 2,095 | 16047.23 | 16187.54 | -0.9% |
| 2,354 | 17891.45 | 18045.04 | -0.9% |
| 2,644 | 20031.93 | 20289.73 | -1.3% |
| 2,970 | 23834.71 | 24041.21 | -0.9% |
| 3,336 | 27678.27 | 27751.31 | -0.3% |
| 3,748 | 31483.39 | 31587.75 | -0.3% |
| 4,210 | 35824.17 | 35846.43 | -0.1% |
| 4,729 | 40426.48 | 40415.77 | +0.0% |
| 5,312 | 45434.83 | 45355.81 | +0.2% |
| 5,967 | 53998.22 | 53877.29 | +0.2% |
| 6,703 | 61422.39 | 61600.41 | -0.3% |
| 7,530 | 69138.90 | 69591.22 | -0.6% |
| 8,458 | 79488.65 | 80091.53 | -0.8% |
| 9,502 | 86966.44 | 87752.17 | -0.9% |
| 10,673 | 98919.65 | 99917.17 | -1.0% |
| 11,990 | 115539.35 | 116230.84 | -0.6% |
| 13,468 | 131891.52 | 132839.62 | -0.7% |
| 15,129 | 148797.50 | 149339.82 | -0.4% |
| 16,995 | 169627.22 | 169803.56 | -0.1% |
| 19,091 | 189593.72 | 189445.23 | +0.1% |
| 21,445 | 214333.24 | 213832.87 | +0.2% |
| 24,090 | 249248.66 | 248631.50 | +0.2% |
| 27,061 | 281791.82 | 283254.28 | -0.5% |
| 30,398 | 314906.11 | 317389.74 | -0.8% |
| 34,147 | 360765.58 | 364028.53 | -0.9% |
| 38,358 | 394039.56 | 397675.38 | -0.9% |
| 43,089 | 447941.12 | 452986.71 | -1.1% |
| 48,403 | 523661.87 | 527452.81 | -0.7% |
| 54,372 | 599130.37 | 603589.16 | -0.7% |
| 61,077 | 674593.35 | 677673.68 | -0.5% |
| 68,609 | 769919.67 | 778695.52 | -1.1% |
| 77,071 | 862552.29 | 870534.42 | -0.9% |
| 86,575 | 975471.63 | 979716.61 | -0.4% |
| 97,252 | 1125463.30 | 1144131.77 | -1.6% |
| 109,246 | 1282840.15 | 1309386.07 | -2.0% |
| 122,718 | 1444844.86 | 1482960.65 | -2.6% |
| 137,852 | 1679760.65 | 1724997.10 | -2.6% |
| 154,853 | 1852549.14 | 1914507.53 | -3.2% |
| 173,950 | 2113110.90 | 2170198.51 | -2.6% |
| 195,402 | 2457662.38 | 2503625.61 | -1.8% |
| 219,500 | 2824851.51 | 2864258.12 | -1.4% |
| 246,569 | 3178116.75 | 3206556.45 | -0.9% |
| 276,977 | 3722624.11 | 3730590.73 | -0.2% |
| 311,135 | 4176436.38 | 4184240.15 | -0.2% |
| 349,506 | 4723357.88 | 4743409.86 | -0.4% |
| 392,608 | 5463788.03 | 5481334.00 | -0.3% |
| 441,026 | 6157737.99 | 6205291.66 | -0.8% |
| 495,415 | 6916405.83 | 6974247.06 | -0.8% |
| 556,512 | 7880936.08 | 7974009.10 | -1.2% |
| 625,143 | 8738505.74 | 8813208.86 | -0.8% |
| 702,238 | 9923655.36 | 9989849.64 | -0.7% |
| 788,841 | 11514629.17 | 11554827.87 | -0.3% |
| 886,124 | 13055625.79 | 13054312.11 | +0.0% |
| 995,405 | 14725154.50 | 14753095.74 | -0.2% |
| 1,118,162 | 16894102.19 | 16902784.54 | -0.1% |
| 1,256,058 | 18872298.43 | 18738916.65 | **+0.7%** |
| 1,410,960 | 21356468.43 | 21330714.63 | +0.1% |
| 1,584,966 | 24388528.25 | 24382653.76 | +0.0% |
| 1,780,430 | 27793921.68 | 27812851.69 | -0.1% |
| 2,000,000 | 30784159.39 | 30978766.30 | -0.6% |

#### IntNaked (Sorted)
| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |
|------|-------------------|-------------------|------|
| 2 | 3.20 | 4.39 | -27.0% |
| 3 | 3.69 | 5.09 | -27.4% |
| 4 | 4.64 | 6.08 | -23.7% |
| 5 | 5.33 | 6.47 | -17.6% |
| 6 | 5.82 | 6.28 | -7.3% |
| 7 | 7.04 | 7.21 | -2.4% |
| 8 | 8.58 | 8.77 | -2.1% |
| 9 | 11.16 | 9.83 | **+13.6%** |
| 10 | 9.90 | 9.98 | -0.8% |
| 20 | 42.55 | 32.29 | **+31.8%** |
| 22 | 52.93 | 38.65 | **+36.9%** |
| 25 | 59.01 | 40.87 | **+44.4%** |
| 28 | 62.03 | 48.10 | **+29.0%** |
| 32 | 77.75 | 50.94 | **+52.6%** |
| 36 | 88.25 | 57.53 | **+53.4%** |
| 40 | 93.72 | 57.93 | **+61.8%** |
| 45 | 119.21 | 73.28 | **+62.7%** |
| 51 | 135.51 | 77.13 | **+75.7%** |
| 57 | 145.28 | 84.21 | **+72.5%** |
| 64 | 180.15 | 94.92 | **+89.8%** |
| 72 | 215.21 | 106.88 | **+101.3%** |
| 81 | 274.66 | 118.16 | **+132.4%** |
| 91 | 303.63 | 135.22 | **+124.5%** |
| 102 | 342.80 | 149.02 | **+130.0%** |
| 114 | 375.48 | 169.59 | **+121.4%** |
| 129 | 435.49 | 180.84 | **+140.8%** |
| 144 | 501.26 | 200.26 | **+150.3%** |
| 162 | 599.73 | 216.64 | **+176.8%** |
| 182 | 672.20 | 246.58 | **+172.6%** |
| 205 | 793.36 | 285.99 | **+177.4%** |
| 230 | 957.07 | 313.46 | **+205.3%** |
| 258 | 1088.80 | 354.70 | **+207.0%** |
| 290 | 1320.49 | 406.60 | **+224.8%** |
| 326 | 1590.12 | 453.15 | **+250.9%** |
| 366 | 1740.93 | 516.14 | **+237.3%** |
| 411 | 1978.05 | 586.40 | **+237.3%** |
| 462 | 2299.19 | 660.39 | **+248.2%** |
| 519 | 2579.11 | 734.80 | **+251.0%** |
| 583 | 3011.07 | 831.34 | **+262.2%** |
| 655 | 3462.18 | 894.91 | **+286.9%** |
| 736 | 3951.98 | 1045.14 | **+278.1%** |
| 826 | 4596.82 | 1205.16 | **+281.4%** |
| 928 | 5452.65 | 1365.98 | **+299.2%** |
| 1,043 | 6358.93 | 1514.54 | **+319.9%** |
| 1,171 | 7385.74 | 1761.84 | **+319.2%** |
| 1,316 | 8527.16 | 1963.67 | **+334.2%** |
| 1,478 | 9515.63 | 2226.72 | **+327.3%** |
| 1,660 | 10901.77 | 2519.53 | **+332.7%** |
| 1,865 | 12540.32 | 2834.48 | **+342.4%** |
| 2,095 | 14114.13 | 3178.80 | **+344.0%** |
| 2,354 | 16267.96 | 3620.10 | **+349.4%** |
| 2,644 | 18667.84 | 3933.71 | **+374.6%** |
| 2,970 | 21260.16 | 4539.26 | **+368.4%** |
| 3,336 | 24650.70 | 5199.30 | **+374.1%** |
| 3,748 | 28597.42 | 5840.98 | **+389.6%** |
| 4,210 | 32582.70 | 6812.76 | **+378.3%** |
| 4,729 | 38239.29 | 8019.96 | **+376.8%** |
| 5,312 | 43887.56 | 9151.60 | **+379.6%** |
| 5,967 | 48837.00 | 10563.83 | **+362.3%** |
| 6,703 | 56223.25 | 12348.02 | **+355.3%** |
| 7,530 | 64698.82 | 14223.71 | **+354.9%** |
| 8,458 | 73168.06 | 16155.88 | **+352.9%** |
| 9,502 | 83953.72 | 18570.08 | **+352.1%** |
| 10,673 | 95530.67 | 21138.07 | **+351.9%** |
| 11,990 | 108302.07 | 24142.46 | **+348.6%** |
| 13,468 | 124683.87 | 27754.59 | **+349.2%** |
| 15,129 | 143339.67 | 30819.72 | **+365.1%** |
| 16,995 | 162517.08 | 34992.05 | **+364.4%** |
| 19,091 | 189152.70 | 39954.28 | **+373.4%** |
| 21,445 | 214283.04 | 44853.02 | **+377.7%** |
| 24,090 | 240844.87 | 51074.04 | **+371.6%** |
| 27,061 | 273932.60 | 57985.20 | **+372.4%** |
| 30,398 | 315001.85 | 64003.04 | **+392.2%** |
| 34,147 | 352822.01 | 71925.07 | **+390.5%** |
| 38,358 | 404297.21 | 81868.02 | **+393.8%** |
| 43,089 | 459423.86 | 92585.96 | **+396.2%** |
| 48,403 | 521483.74 | 106768.28 | **+388.4%** |
| 54,372 | 599268.78 | 121290.71 | **+394.1%** |
| 61,077 | 691016.64 | 136980.31 | **+404.5%** |
| 68,609 | 787448.26 | 155478.05 | **+406.5%** |
| 77,071 | 910852.76 | 179661.77 | **+407.0%** |
| 86,575 | 1027155.79 | 202428.10 | **+407.4%** |
| 97,252 | 1151020.18 | 228595.76 | **+403.5%** |
| 109,246 | 1295287.35 | 260915.43 | **+396.4%** |
| 122,718 | 1493299.32 | 284799.68 | **+424.3%** |
| 137,852 | 1686962.66 | 325028.27 | **+419.0%** |
| 154,853 | 1935448.32 | 364200.61 | **+431.4%** |
| 173,950 | 2178096.18 | 415678.62 | **+424.0%** |
| 195,402 | 2481614.95 | 476920.23 | **+420.3%** |
| 219,500 | 2846565.72 | 543395.37 | **+423.8%** |
| 246,569 | 3240930.75 | 606725.41 | **+434.2%** |
| 276,977 | 3722740.91 | 696509.82 | **+434.5%** |
| 311,135 | 4303498.98 | 804180.47 | **+435.1%** |
| 349,506 | 4839842.24 | 903490.36 | **+435.7%** |
| 392,608 | 5426054.26 | 1025031.47 | **+429.4%** |
| 441,026 | 6162699.56 | 1157418.17 | **+432.5%** |
| 495,415 | 7047251.69 | 1285861.52 | **+448.1%** |
| 556,512 | 7917938.20 | 1439374.22 | **+450.1%** |
| 625,143 | 9063433.99 | 1632643.60 | **+455.1%** |
| 702,238 | 10205370.17 | 1861683.55 | **+448.2%** |
| 788,841 | 11595124.30 | 2131784.17 | **+443.9%** |
| 886,124 | 13228137.56 | 2424513.94 | **+445.6%** |
| 995,405 | 15069597.83 | 2718047.48 | **+454.4%** |
| 1,118,162 | 17228463.41 | 3111752.95 | **+453.7%** |
| 1,256,058 | 19754055.94 | 3612497.23 | **+446.8%** |
| 1,410,960 | 22184236.97 | 4100930.15 | **+441.0%** |
| 1,584,966 | 25026958.35 | 4658814.88 | **+437.2%** |
| 1,780,430 | 28466978.35 | 5290345.46 | **+438.1%** |
| 2,000,000 | 32200924.23 | 5855186.97 | **+450.0%** |

#### WideRow (Random)
| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |
|------|-------------------|-------------------|------|
| 2 | 196.21 | 199.46 | -1.6% |
| 3 | 268.63 | 270.17 | -0.6% |
| 4 | 350.29 | 349.75 | +0.2% |
| 5 | 471.95 | 462.73 | **+2.0%** |
| 6 | 813.76 | 802.64 | **+1.4%** |
| 7 | 785.27 | 789.44 | -0.5% |
| 8 | 887.81 | 892.38 | -0.5% |
| 9 | 1006.86 | 1010.78 | -0.4% |
| 10 | 1231.94 | 1235.51 | -0.3% |
| 20 | 5156.10 | 5216.27 | -1.2% |
| 22 | 5674.63 | 5724.96 | -0.9% |
| 25 | 6590.27 | 6580.18 | +0.2% |
| 28 | 7197.75 | 7240.10 | -0.6% |
| 32 | 8334.19 | 8313.75 | +0.2% |
| 36 | 9276.28 | 9315.19 | -0.4% |
| 40 | 10222.36 | 10285.50 | -0.6% |
| 45 | 11606.90 | 11599.33 | +0.1% |
| 51 | 13148.38 | 13156.44 | -0.1% |
| 57 | 14703.10 | 14758.43 | -0.4% |
| 64 | 16494.51 | 16468.90 | +0.2% |
| 72 | 18564.05 | 18641.38 | -0.4% |
| 81 | 21010.96 | 20911.07 | +0.5% |
| 91 | 23726.43 | 23758.75 | -0.1% |
| 102 | 26417.98 | 26548.58 | -0.5% |
| 114 | 29819.47 | 29692.80 | +0.4% |
| 129 | 33691.03 | 33531.86 | +0.5% |
| 144 | 37654.24 | 37419.26 | **+0.6%** |
| 162 | 42462.68 | 42185.33 | **+0.7%** |
| 182 | 47858.77 | 47502.22 | **+0.8%** |
| 205 | 53873.43 | 53674.53 | +0.4% |
| 230 | 60644.44 | 60296.57 | **+0.6%** |
| 258 | 67820.57 | 68071.91 | -0.4% |
| 290 | 77076.19 | 76621.79 | **+0.6%** |
| 326 | 110696.86 | 110689.04 | +0.0% |
| 366 | 151593.82 | 151533.30 | +0.0% |
| 411 | 171228.20 | 170464.28 | +0.4% |
| 462 | 193951.85 | 191432.92 | **+1.3%** |
| 519 | 217416.62 | 216289.48 | **+0.5%** |
| 583 | 245284.63 | 243772.05 | **+0.6%** |
| 655 | 276676.69 | 274190.59 | **+0.9%** |
| 736 | 310135.02 | 307871.63 | **+0.7%** |
| 826 | 347505.62 | 345774.96 | **+0.5%** |
| 928 | 393682.78 | 390148.92 | **+0.9%** |
| 1,043 | 445681.74 | 441193.78 | **+1.0%** |
| 1,171 | 498890.41 | 498256.13 | +0.1% |
| 1,316 | 650528.14 | 643786.04 | **+1.0%** |
| 1,478 | 865601.63 | 873158.95 | -0.9% |
| 1,660 | 993990.69 | 968753.15 | **+2.6%** |
| 1,865 | 1111171.09 | 1101577.93 | **+0.9%** |
| 2,095 | 1253111.01 | 1254224.71 | -0.1% |
| 2,354 | 1442669.80 | 1438658.05 | +0.3% |
| 2,644 | 1657209.32 | 1650759.34 | +0.4% |
| 2,970 | 1920143.74 | 1915299.20 | +0.3% |
| 3,336 | 2280165.85 | 2297121.91 | -0.7% |
| 3,748 | 2700092.50 | 2702560.86 | -0.1% |
| 4,210 | 3222051.18 | 3194279.09 | **+0.9%** |
| 4,729 | 3761192.38 | 3775276.58 | -0.4% |
| 5,312 | 4911601.95 | 4930203.27 | -0.4% |
| 5,967 | 6047164.49 | 6033545.29 | +0.2% |
| 6,703 | 6988356.43 | 6962009.17 | +0.4% |
| 7,530 | 8028386.84 | 7982040.25 | **+0.6%** |
| 8,458 | 9226421.61 | 9226842.10 | -0.0% |
| 9,502 | 10640738.64 | 10601986.12 | +0.4% |
| 10,673 | 12180747.12 | 12136856.33 | +0.4% |
| 11,990 | 13863519.18 | 13847011.43 | +0.1% |
| 13,468 | 15895272.73 | 15972825.40 | -0.5% |
| 15,129 | 18176891.03 | 18208113.24 | -0.2% |
| 16,995 | 20787506.34 | 20774180.14 | +0.1% |
| 19,091 | 23604205.57 | 23613888.86 | -0.0% |
| 21,445 | 29113979.17 | 29189015.62 | -0.3% |
| 24,090 | 33849734.14 | 33673262.50 | **+0.5%** |
| 27,061 | 38505423.61 | 38094004.62 | **+1.1%** |
| 30,398 | 43132830.75 | 42906557.31 | **+0.5%** |
| 34,147 | 48656131.01 | 48559093.01 | +0.2% |
| 38,358 | 54768423.57 | 54642868.00 | +0.2% |
| 43,089 | 61813265.11 | 61674458.36 | +0.2% |
| 48,403 | 69912199.11 | 69551287.01 | **+0.5%** |
| 54,372 | 79070041.77 | 79262505.26 | -0.2% |
| 61,077 | 89591273.71 | 89225357.17 | +0.4% |
| 68,609 | 101328416.69 | 101064368.00 | +0.3% |
| 77,071 | 114544069.51 | 114097298.47 | +0.4% |
| 86,575 | 141071874.97 | 140767739.50 | +0.2% |
| 97,252 | 160603864.47 | 160401562.46 | +0.1% |
| 109,246 | 183491472.39 | 181677513.68 | **+1.0%** |
| 122,718 | 206051791.71 | 205602166.63 | +0.2% |
| 137,852 | 233092208.32 | 232521749.96 | +0.2% |
| 154,853 | 263646979.00 | 263027354.49 | +0.2% |
| 173,950 | 298377937.52 | 297044271.02 | +0.4% |
| 195,402 | 336181812.57 | 335520187.39 | +0.2% |
| 219,500 | 380412583.01 | 381722958.06 | -0.3% |
| 246,569 | 430702395.97 | 430064228.95 | +0.1% |
| 276,977 | 487349000.06 | 486834208.02 | +0.1% |
| 311,135 | 549183625.03 | 548234499.99 | +0.2% |
| 349,506 | 673042875.02 | 672246541.83 | +0.1% |
| 392,608 | 759909625.17 | 757415249.94 | +0.3% |
| 441,026 | 860850624.97 | 858308749.97 | +0.3% |
| 495,415 | 969862249.91 | 968354500.13 | +0.2% |
| 556,512 | 1093857917.00 | 1094260417.16 | -0.0% |
| 625,143 | 1237333667.00 | 1245250334.03 | -0.6% |
| 702,238 | 1398262457.92 | 1395872582.91 | +0.2% |
| 788,841 | 1578608209.03 | 1576094500.02 | +0.2% |
| 886,124 | 1785145916.04 | 1781267750.08 | +0.2% |
| 995,405 | 2022511500.17 | 2015425790.80 | +0.4% |
| 1,118,162 | 2532810500.12 | 2457166707.84 | **+3.1%** |
| 1,256,058 | 2847431457.83 | 2829548500.01 | **+0.6%** |
| 1,410,960 | 3422080624.85 | 3407984958.03 | +0.4% |
| 1,584,966 | 3813689999.98 | 3839397166.86 | -0.7% |
| 1,780,430 | 4349527208.13 | 4340965582.99 | +0.2% |
| 2,000,000 | 4914486333.96 | 4900332083.23 | +0.3% |

#### WideRow (Reverse)
| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |
|------|-------------------|-------------------|------|
| 2 | 195.90 | 199.23 | -1.7% |
| 3 | 268.27 | 270.07 | -0.7% |
| 4 | 348.53 | 351.50 | -0.8% |
| 5 | 389.14 | 400.02 | -2.7% |
| 6 | 736.66 | 731.89 | **+0.7%** |
| 7 | 848.93 | 851.13 | -0.3% |
| 8 | 1120.97 | 1124.24 | -0.3% |
| 9 | 1231.43 | 1237.31 | -0.5% |
| 10 | 1459.37 | 1462.46 | -0.2% |
| 20 | 5436.02 | 5396.01 | **+0.7%** |
| 22 | 5929.20 | 5886.32 | **+0.7%** |
| 25 | 6674.47 | 6605.15 | **+1.0%** |
| 28 | 7428.18 | 7356.59 | **+1.0%** |
| 32 | 8425.30 | 8339.31 | **+1.0%** |
| 36 | 9482.61 | 9385.77 | **+1.0%** |
| 40 | 10414.49 | 10386.08 | +0.3% |
| 45 | 11739.11 | 11701.72 | +0.3% |
| 51 | 13245.63 | 13204.86 | +0.3% |
| 57 | 14685.88 | 14580.48 | **+0.7%** |
| 64 | 16375.78 | 16371.55 | +0.0% |
| 72 | 18408.41 | 18337.73 | +0.4% |
| 81 | 20713.86 | 20584.78 | **+0.6%** |
| 91 | 23419.81 | 23298.96 | **+0.5%** |
| 102 | 26457.88 | 26314.82 | **+0.5%** |
| 114 | 29458.68 | 29205.36 | **+0.9%** |
| 129 | 32958.70 | 32782.05 | **+0.5%** |
| 144 | 37094.61 | 36718.26 | **+1.0%** |
| 162 | 41387.31 | 41071.03 | **+0.8%** |
| 182 | 46654.20 | 46312.35 | **+0.7%** |
| 205 | 52614.99 | 52637.70 | -0.0% |
| 230 | 58497.36 | 58348.68 | +0.3% |
| 258 | 65948.05 | 65349.61 | **+0.9%** |
| 290 | 73981.97 | 73713.43 | +0.4% |
| 326 | 108248.53 | 107798.47 | +0.4% |
| 366 | 149230.92 | 147784.86 | **+1.0%** |
| 411 | 168593.36 | 167320.59 | **+0.8%** |
| 462 | 188691.44 | 187818.93 | +0.5% |
| 519 | 215160.73 | 216310.60 | -0.5% |
| 583 | 239068.81 | 238047.82 | +0.4% |
| 655 | 268874.34 | 267457.22 | **+0.5%** |
| 736 | 300531.14 | 297900.60 | **+0.9%** |
| 826 | 337811.16 | 336471.60 | +0.4% |
| 928 | 383505.29 | 376483.17 | **+1.9%** |
| 1,043 | 431322.21 | 432524.19 | -0.3% |
| 1,171 | 490226.81 | 486148.90 | **+0.8%** |
| 1,316 | 628220.76 | 622400.41 | **+0.9%** |
| 1,478 | 844202.01 | 837181.08 | **+0.8%** |
| 1,660 | 956830.32 | 947062.01 | **+1.0%** |
| 1,865 | 1087414.59 | 1079980.47 | **+0.7%** |
| 2,095 | 1231412.80 | 1227902.08 | +0.3% |
| 2,354 | 1402325.74 | 1394638.05 | **+0.6%** |
| 2,644 | 1615346.05 | 1600764.31 | **+0.9%** |
| 2,970 | 1907254.38 | 1888772.30 | **+1.0%** |
| 3,336 | 2256768.88 | 2256814.33 | -0.0% |
| 3,748 | 2660410.35 | 2654962.63 | +0.2% |
| 4,210 | 3133274.55 | 3134082.01 | -0.0% |
| 4,729 | 3718408.24 | 3726164.67 | -0.2% |
| 5,312 | 4790381.56 | 4819472.41 | -0.6% |
| 5,967 | 5936963.63 | 5923024.78 | +0.2% |
| 6,703 | 6858376.64 | 6822072.20 | **+0.5%** |
| 7,530 | 7993002.98 | 7880143.94 | **+1.4%** |
| 8,458 | 9118606.91 | 9008313.86 | **+1.2%** |
| 9,502 | 10451722.23 | 10423125.63 | +0.3% |
| 10,673 | 12013330.45 | 11936308.61 | **+0.6%** |
| 11,990 | 13720708.34 | 13668688.73 | +0.4% |
| 13,468 | 15731390.14 | 15689120.38 | +0.3% |
| 15,129 | 18085439.68 | 17977723.28 | **+0.6%** |
| 16,995 | 20603051.47 | 20650762.26 | -0.2% |
| 19,091 | 23453429.17 | 23456320.83 | -0.0% |
| 21,445 | 29054376.71 | 28940137.17 | +0.4% |
| 24,090 | 33847833.34 | 33778940.47 | +0.2% |
| 27,061 | 38368449.12 | 37993812.51 | **+1.0%** |
| 30,398 | 42948914.07 | 42785825.56 | +0.4% |
| 34,147 | 48435949.42 | 48155118.99 | **+0.6%** |
| 38,358 | 54451718.75 | 54184409.65 | +0.5% |
| 43,089 | 61822212.51 | 61500695.79 | **+0.5%** |
| 48,403 | 70159620.45 | 69868888.89 | +0.4% |
| 54,372 | 78895145.85 | 78692697.86 | +0.3% |
| 61,077 | 90041101.15 | 89988172.72 | +0.1% |
| 68,609 | 101797625.00 | 101668041.67 | +0.1% |
| 77,071 | 114666641.60 | 114604975.00 | +0.1% |
| 86,575 | 141218875.01 | 141056260.50 | +0.1% |
| 97,252 | 161042312.51 | 160539291.74 | +0.3% |
| 109,246 | 182977972.40 | 181678718.71 | **+0.7%** |
| 122,718 | 206588458.35 | 205744291.66 | +0.4% |
| 137,852 | 234528444.32 | 233566389.03 | +0.4% |
| 154,853 | 264935728.98 | 263952687.50 | +0.4% |
| 173,950 | 300542145.50 | 299376770.97 | +0.4% |
| 195,402 | 339189562.48 | 339626437.52 | -0.1% |
| 219,500 | 384360875.00 | 383951333.47 | +0.1% |
| 246,569 | 435937333.04 | 435760166.96 | +0.0% |
| 276,977 | 501069291.96 | 492438416.93 | **+1.8%** |
| 311,135 | 555550500.05 | 554055042.10 | +0.3% |
| 349,506 | 678681791.99 | 676240083.06 | +0.4% |
| 392,608 | 760330417.19 | 761349999.81 | -0.1% |
| 441,026 | 867158500.01 | 870289416.98 | -0.4% |
| 495,415 | 984670790.84 | 980560458.96 | +0.4% |
| 556,512 | 1114561709.11 | 1102539792.19 | **+1.1%** |
| 625,143 | 1252316625.09 | 1248233708.09 | +0.3% |
| 702,238 | 1419163665.97 | 1421945209.15 | -0.2% |
| 788,841 | 1601427250.08 | 1594524832.91 | +0.4% |
| 886,124 | 1813165457.92 | 1810292500.07 | +0.2% |
| 995,405 | 2063021666.84 | 2060930707.96 | +0.1% |
| 1,118,162 | 2552567416.10 | 2538337999.95 | **+0.6%** |
| 1,256,058 | 2895854041.91 | 2879240792.01 | **+0.6%** |
| 1,410,960 | 3472940916.89 | 3467443166.07 | +0.2% |
| 1,584,966 | 3905677624.98 | 3911712083.97 | -0.2% |
| 1,780,430 | 4419071583.08 | 4414050541.12 | +0.1% |
| 2,000,000 | 4986328291.94 | 4972068709.09 | +0.3% |

#### WideRow (Sorted)
| Size | High Entropy (ns) | Adaptive Mode (ns) | Gain |
|------|-------------------|-------------------|------|
| 2 | 84.63 | 84.11 | **+0.6%** |
| 3 | 123.59 | 120.85 | **+2.3%** |
| 4 | 174.42 | 168.58 | **+3.5%** |
| 5 | 232.49 | 211.17 | **+10.1%** |
| 6 | 285.72 | 259.32 | **+10.2%** |
| 7 | 328.95 | 310.88 | **+5.8%** |
| 8 | 383.42 | 386.32 | -0.8% |
| 9 | 421.85 | 424.93 | -0.7% |
| 10 | 467.77 | 471.58 | -0.8% |
| 20 | 5648.55 | 5618.41 | **+0.5%** |
| 22 | 6079.30 | 6119.48 | -0.7% |
| 25 | 6882.52 | 6829.21 | **+0.8%** |
| 28 | 7407.25 | 7556.90 | -2.0% |
| 32 | 8577.57 | 8437.78 | **+1.7%** |
| 36 | 9432.50 | 9462.58 | -0.3% |
| 40 | 10414.81 | 10379.76 | +0.3% |
| 45 | 11801.07 | 11749.77 | +0.4% |
| 51 | 13151.25 | 13148.58 | +0.0% |
| 57 | 14581.85 | 14590.33 | -0.1% |
| 64 | 16476.69 | 16245.74 | **+1.4%** |
| 72 | 18350.28 | 18138.94 | **+1.2%** |
| 81 | 20514.21 | 20431.56 | +0.4% |
| 91 | 23386.23 | 22738.30 | **+2.8%** |
| 102 | 26022.52 | 25336.54 | **+2.7%** |
| 114 | 28808.70 | 28515.03 | **+1.0%** |
| 129 | 32305.82 | 32066.65 | **+0.7%** |
| 144 | 36407.27 | 35635.61 | **+2.2%** |
| 162 | 40732.72 | 40002.88 | **+1.8%** |
| 182 | 45711.97 | 44629.50 | **+2.4%** |
| 205 | 51242.66 | 50832.96 | **+0.8%** |
| 230 | 58026.97 | 56873.15 | **+2.0%** |
| 258 | 65028.47 | 63430.20 | **+2.5%** |
| 290 | 72923.17 | 71427.81 | **+2.1%** |
| 326 | 107772.50 | 105627.80 | **+2.0%** |
| 366 | 147972.88 | 144983.13 | **+2.1%** |
| 411 | 166569.67 | 164194.91 | **+1.4%** |
| 462 | 186228.32 | 184239.41 | **+1.1%** |
| 519 | 209723.21 | 207285.18 | **+1.2%** |
| 583 | 235941.94 | 231544.92 | **+1.9%** |
| 655 | 264866.75 | 260571.03 | **+1.6%** |
| 736 | 297048.14 | 297963.96 | -0.3% |
| 826 | 333335.94 | 329941.10 | **+1.0%** |
| 928 | 375837.66 | 380151.13 | -1.1% |
| 1,043 | 435498.26 | 421859.41 | **+3.2%** |
| 1,171 | 489609.98 | 476715.18 | **+2.7%** |
| 1,316 | 629218.70 | 618293.90 | **+1.8%** |
| 1,478 | 841977.69 | 849836.21 | -0.9% |
| 1,660 | 951183.11 | 948745.29 | +0.3% |
| 1,865 | 1085045.07 | 1068150.17 | **+1.6%** |
| 2,095 | 1231681.90 | 1217486.50 | **+1.2%** |
| 2,354 | 1421059.69 | 1404808.78 | **+1.2%** |
| 2,644 | 1635766.23 | 1633656.03 | +0.1% |
| 2,970 | 1945755.03 | 1925151.46 | **+1.1%** |
| 3,336 | 2276772.01 | 2283503.15 | -0.3% |
| 3,748 | 2666436.29 | 2653571.55 | +0.5% |
| 4,210 | 3109053.12 | 3108096.54 | +0.0% |
| 4,729 | 3626468.53 | 3636536.24 | -0.3% |
| 5,312 | 4766469.46 | 4759868.43 | +0.1% |
| 5,967 | 5850969.64 | 5848729.87 | +0.0% |
| 6,703 | 6726232.20 | 6707241.91 | +0.3% |
| 7,530 | 7770677.90 | 7759987.83 | +0.1% |
| 8,458 | 8918923.16 | 8909180.74 | +0.1% |
| 9,502 | 10249032.47 | 10197858.21 | **+0.5%** |
| 10,673 | 11694887.50 | 11624110.42 | **+0.6%** |
| 11,990 | 13364207.52 | 13340387.02 | +0.2% |
| 13,468 | 15348100.54 | 15529488.22 | -1.2% |
| 15,129 | 17534692.73 | 17641151.72 | -0.6% |
| 16,995 | 20040964.29 | 20110225.00 | -0.3% |
| 19,091 | 22774611.55 | 22799275.55 | -0.1% |
| 21,445 | 28536650.00 | 28302460.00 | **+0.8%** |
| 24,090 | 33006644.86 | 32921585.29 | +0.3% |
| 27,061 | 37228782.89 | 37250223.68 | -0.1% |
| 30,398 | 41964007.35 | 41958960.83 | +0.0% |
| 34,147 | 47357767.85 | 47289491.07 | +0.1% |
| 38,358 | 53409468.75 | 53292680.49 | +0.2% |
| 43,089 | 60245983.40 | 60195261.36 | +0.1% |
| 48,403 | 68029963.00 | 67889562.51 | +0.2% |
| 54,372 | 76872416.63 | 77176489.63 | -0.4% |
| 61,077 | 87095404.71 | 87393381.00 | -0.3% |
| 68,609 | 98539118.00 | 99133361.17 | -0.6% |
| 77,071 | 112029354.17 | 111983062.51 | +0.0% |
| 86,575 | 139032520.99 | 138762466.61 | +0.2% |
| 97,252 | 158444812.52 | 158526812.51 | -0.1% |
| 109,246 | 179359736.00 | 179806603.99 | -0.2% |
| 122,718 | 202679847.29 | 202763305.67 | -0.0% |
| 137,852 | 229930541.63 | 229816388.96 | +0.0% |
| 154,853 | 259822041.49 | 261355569.35 | -0.6% |
| 173,950 | 292766479.43 | 293486771.06 | -0.2% |
| 195,402 | 332209312.47 | 332134666.50 | +0.0% |
| 219,500 | 374376958.59 | 375712395.99 | -0.4% |
| 246,569 | 425011479.06 | 425554228.95 | -0.1% |
| 276,977 | 479216666.89 | 481134208.85 | -0.4% |
| 311,135 | 542142832.88 | 544174415.99 | -0.4% |
| 349,506 | 666828874.96 | 666300083.98 | +0.1% |
| 392,608 | 748713959.01 | 754762500.06 | -0.8% |
| 441,026 | 849978875.14 | 853405707.98 | -0.4% |
| 495,415 | 969812333.12 | 968418583.04 | +0.1% |
| 556,512 | 1086262749.97 | 1087653666.97 | -0.1% |
| 625,143 | 1226992417.13 | 1230885915.92 | -0.3% |
| 702,238 | 1383208832.94 | 1385815792.02 | -0.2% |
| 788,841 | 1560362666.15 | 1567254458.09 | -0.4% |
| 886,124 | 1765334625.03 | 1773190957.96 | -0.4% |
| 995,405 | 1998090500.01 | 2002842457.97 | -0.2% |
| 1,118,162 | 2516384166.90 | 2453428125.01 | **+2.6%** |
| 1,256,058 | 2826208458.05 | 2826350042.13 | -0.0% |
| 1,410,960 | 3412763791.16 | 3408280250.15 | +0.1% |
| 1,584,966 | 3836051999.82 | 3853470166.91 | -0.5% |
| 1,780,430 | 4339969832.92 | 4350271791.92 | -0.2% |
| 2,000,000 | 4899896624.96 | 4913972374.98 | -0.3% |
