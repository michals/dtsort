# dtsort Sorter Performance: Executive Summary

- **Timestamp**: `2026-05-27T04:37:13.236627`
- **Git Commit**: `d41b20c`
- **Mode**: `gs` (Block Size: `160`, sizeof(T) > 32 Threshold)

## 1. Peak Speedup Cheat Sheet

The table below displays the maximum speedup gains achieved by `dt_stable_sort` (High Entropy / Adaptive Mode) relative to standard `std::stable_sort` on large arrays ($N \ge 30$).

| Type | Distribution | Peak Speedup (High Entropy) | Optimal Size (N) | Peak Speedup (Adaptive Mode) | Optimal Size (N) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **IntNaked** | Random | **+24.6%** | 40 | **+24.9%** | 72 |
| **IntNaked** | Sorted | **+44.6%** | 72 | **+444.3%** | 4,729 |
| **IntNaked** | Reversed | **+36.0%** | 40 | **+41.3%** | 40 |
| **WideRow** | Random | **+142.4%** | 290 | **+143.4%** | 290 |
| **WideRow** | Sorted | **+137.6%** | 290 | **+153.4%** | 290 |
| **WideRow** | Reversed | **+155.9%** | 258 | **+159.2%** | 258 |

> **Note on Move Savings**: The reported $1.0N$ moves are bounded and occur specifically within the cache-aligned group index sorting phase for heavy objects (`sizeof(T) > 32` bytes) within each block of size $M = 160$ elements.

## 2. Sorter Characteristics Table

Complete theoretical and code size breakdown for each generated decision tree module ($N = 2..10$).

| Sorter | Mode / Split | Worst-Case Cmp | Avg Cmp | Avg Move | Max Move | Header Size (KB) | Object Size (KB) |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| `dtsort2` | `full` | 1 | 1.00 | 1.50 | 3 | 1.19 KB | 0.56 KB |
| `dtsort3` | `full` | 3 | 2.67 | 2.83 | 4 | 1.38 KB | 0.66 KB |
| `dtsort4` | `full` | 5 | 4.67 | 4.08 | 6 | 2.44 KB | 1.09 KB |
| `dtsort5` | `full` | 7 | 6.93 | 5.28 | 7 | 6.88 KB | 3.24 KB |
| `dtsort6` | `gs42` | 11 | 10.00 | 9.47 | 14 | 2.44 KB | 1.70 KB |
| `dtsort7` | `gs52` | 13 | 12.48 | 11.24 | 16 | 2.85 KB | 3.75 KB |
| `dtsort8` | `gs53` | 17 | 15.89 | 13.71 | 19 | 5.42 KB | 4.98 KB |
| `dtsort9` | `gs54` | 20 | 19.08 | 15.95 | 22 | 11.87 KB | 7.70 KB |
| `dtsort10` | `gs55` | 23 | 22.08 | 18.06 | 25 | 25.94 KB | 14.32 KB |