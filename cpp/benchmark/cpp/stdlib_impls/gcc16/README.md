# GCC 16 stable_sort Extraction

This directory contains the extracted `stable_sort` implementation from GCC 16 libstdc++.

## Source Origin
- **Source**: GCC 16.1.0 release headers (`bits/stl_algo.h`)

## License
The files in `original/` are snapshots of the GCC standard library (libstdc++) and are covered by the **GNU General Public License v3.0 with GCC Runtime Library Exception**.

## Structure

- `original/`: Original headers downloaded from GCC 16 source tree.
- `generated/`: Patched headers for use with `dtsort`.
- `generated_stock/`: Patched headers with stock algorithms (no `dtsort`).
- `adapter.h`: Adapter for patched version (defines required `_GCC16_` macros and shims).
- `patches/`: Contains the `dtsort_logic.patch` for algorithmic changes.
- `patch_dtsort.sh`: Script to generate `generated/` from `original/`.
- `patch_stock.sh`: Script to generate `generated_stock/` from `original/`.

---

# Algorithm Deep-Dive: `std::stable_sort()` in GCC 16

## 1. High-Level Strategy

Unlike LLVM's purely top-down recursive merge sort, GCC 16's `stable_sort` is a **hybrid algorithm** that switches to a **bottom-up merge sort** when enough temporary memory can be allocated. 

It requests a temporary buffer of size $N/2$. Based on the allocation result:

| Buffer Allocation | Strategy | Complexity |
|-------------------|----------|------------|
| `buffer ≥ N/2` | **Bottom-up merge sort** on two halves, then merge | O(N log N) |
| `buffer < N/2` (Partial) | **Top-down recursive split** until sizes fit the partial buffer, then bottom-up | O(N log N) |
| `buffer == 0` | **In-place top-down merge sort** | O(N log²N) |

In the vast majority of cases (where memory allocation succeeds), the fast buffered bottom-up path dominates.

## 2. Entry Point Chain

```
stable_sort(first, last, comp)
  └─ __stable_sort(first, last, comp)
       ├─ Allocate _TmpBuf of size len / 2
       ├─ CASE A: buf == len/2 → __stable_sort_adaptive (Fast bottom-up)
       ├─ CASE B: buf == 0     → __inplace_stable_sort  (Slow in-place)
       └─ CASE C: 0 < buf < len/2 → __stable_sort_adaptive_resize
```

## 3. The Buffered Path (Bottom-Up)

Source: `original/bits/stl_algo.h`

```
__stable_sort_adaptive(first, m, last, buffer, comp):
    ├─ __merge_sort_with_buffer(first, m, buffer, comp)  → Bottom-up sort left half
    ├─ __merge_sort_with_buffer(m, last, buffer, comp)   → Bottom-up sort right half
    └─ __merge_adaptive(first, m, last, ..., buffer)     → Merge halves
```

### The Bottom-Up Core: `__merge_sort_with_buffer`
GCC uses a fixed chunk size for the leaf nodes of its bottom-up merge sort:
```cpp
enum { _S_chunk_size = 7 };
```

```
__merge_sort_with_buffer(first, last, buffer, comp):
    ├─ __chunk_insertion_sort(first, last, _S_chunk_size, comp)
    │    └─ Splits array into chunks of 7 elements.
    │    └─ Sorts each chunk IN-PLACE using __insertion_sort.
    │
    └─ while (step_size < len):  // loop: 7 -> 14 -> 28 -> ...
         ├─ __merge_sort_loop(first, last, buffer, step_size)   → Merge to buffer
         ├─ step_size *= 2
         └─ __merge_sort_loop(buffer, buff_end, first, step_size) → Merge to array
```
> **Key Observation**: The recursion leaf chunks are exactly **7 elements**! This is a perfect match for `dtsort7`.

## 4. The In-Place Path (Top-Down)

```
__inplace_stable_sort(first, last, comp):
    ├─ if (len < 15):
    │    └─ __insertion_sort(first, last, comp) [IN-PLACE]
    │
    ├─ Split: m = first + len/2
    ├─ __inplace_stable_sort(first, m, comp)
    ├─ __inplace_stable_sort(m, last, comp)
    └─ __merge_without_buffer(first, m, last)
```
> **Key Observation**: The leaf size for the in-place fallback path is `< 15` elements.

---

# dtsort Implementation Notes

### Buffered Path vs. In-place Path
GCC 16 `std::stable_sort` has two primary execution paths:
1. **Buffered Merge Sort (`__stable_sort_adaptive`)**: Used when enough memory is available. This is the preferred path and where most optimizations occur. It uses a bottom-up merge approach starting with chunks of size 7 (`_S_chunk_size`).
2. **In-place Merge Sort (`__inplace_stable_sort`)**: Used as a fallback when memory allocation fails. It uses a top-down recursive split and falls back to insertion sort for segments smaller than 15.

### The `_GLIBCXX_HOSTED` Requirement
In the benchmark environment, the GCC 16 headers are compiled in a "shimmed" environment. By default, GCC headers check `_GLIBCXX_HOSTED` to decide whether to use standard library features like `_Temporary_buffer`. If this macro is not defined, `stable_sort` falls back to the in-place path, which is significantly slower and may bypass some `dtsort` hooks if the recursive split doesn't align with `MAX_DT_SIZE`.

**Fix**: We explicitly define `#define _GLIBCXX_HOSTED 1` in `adapter.h` to ensure the buffered path is utilized.

### C++26 Constexpr Guards
GCC 16 introduces several `#if __glibcxx_constexpr_algorithms >= 202306L` blocks that use `if consteval` (C++26 syntax). Since our benchmark environment compiles with C++20, these cause errors. 

**Fix**: We define these in `adapter.h`:
```cpp
#define _GLIBCXX26_CONSTEXPR
#define __glibcxx_constexpr_algorithms 0
```

### Patching Strategy
We've refined the `dtsort_logic.patch` to be more robust:
- **`__chunk_insertion_sort`**: Intercepts the base chunks (size 7) of the buffered merge sort.
- **`__inplace_stable_sort`**: Intercepts the leaf cases of the recursive split.
- **`__final_insertion_sort`**: Intercepts the final pass used in some sort variants.

## Verification
- **Validate mode**: Ensures dtsort correctly sorts small arrays and remains stable.
- **Counts mode**: Verifies that `dtsort_comparisons` and `dtsort_moves` are non-zero for large datasets, indicating successful pre-sorting interception.
