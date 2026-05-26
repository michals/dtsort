# LLVM 15 stable_sort Extraction

This directory contains the extracted `stable_sort` implementation from LLVM 15 libc++.

## Source Origin
- **Source**: LLVM 15.x release headers (`__algorithm/stable_sort.h`, `__algorithm/sort.h`, etc.)

## License
The files in `original/` are snapshots of the LLVM standard library (libc++) and are covered by the **Apache License v2.0 with LLVM Exceptions**.

## Structure

- `original/`: Original headers downloaded from LLVM 15.x release.
- `generated/`: Patched headers for use with `dtsort`.
- `generated_stock/`: Patched headers with stock algorithms (no `dtsort`).
- `adapter.h`: Adapter for patched version (defines required `_LLVM15_` macros and shims).
- `patches/`: Contains the `dtsort_logic.patch` for algorithmic changes.
- `patch_dtsort.sh`: Script to generate `generated/` from `original/`.
- `patch_stock.sh`: Script to generate `generated_stock/` from `original/`.

---

# Algorithm Deep-Dive: `std::stable_sort()` in LLVM 15

## 1. High-Level Strategy

LLVM 15's `stable_sort` implements a **top-down recursive merge sort** utilizing a temporary buffer. The strategy diverges into two modes depending on buffer allocation success:

| Condition | Strategy | Complexity |
|-----------|----------|------------|
| `buffer ≥ len` | **Buffered merge sort** — sort-and-move into buffer, merge back | O(N log N) guaranteed |
| `buffer < len` | **In-place merge sort** — recursive split + `inplace_merge` | O(N log²N) worst case |

Under typical circumstances, allocating a buffer of size `N` via `std::get_temporary_buffer` succeeds, meaning the buffered top-down merge sort represents the primary execution path.

## 2. Entry Point Chain

```
stable_sort(first, last, comp)
  └─ __stable_sort_impl(first, last, comp)
       ├─ Compute len = last - first
       ├─ If len > __stable_sort_switch::value → allocate temporary buffer of size len
       └─ __stable_sort(first, last, comp, len, buff, buff_size)
```

## 3. The `__stable_sort_switch` Threshold

```cpp
template <class _Tp>
struct __stable_sort_switch {
    static const unsigned value = 128 * is_trivially_copy_assignable<_Tp>::value;
};
```

This constant behaves identically to newer LLVM versions:
- **Trivially copy-assignable types** (`int`, `double`, etc.): **128**. For $N \le 128$, `__stable_sort` delegates entirely to in-place insertion sort. Buffer allocation is bypassed for these sizes.
- **Non-trivially copy-assignable types** (`std::string`, complex objects): **0**. The algorithm always attempts to allocate a buffer and never short-circuits to the in-place insertion sort within `__stable_sort`.

## 4. `__stable_sort()` — Main Recursive Function

Source: `original/__algorithm/stable_sort.h:162-207`

The function splits the range recursively, utilizing the buffer optimally.

```
__stable_sort(first, last, comp, len, buff, buff_size):
    ├─ len == 0 or 1 → return
    ├─ len == 2 → compare-and-swap
    ├─ len ≤ __stable_sort_switch::value (128 or 0)
    │    └─ __insertion_sort(first, last, comp) [IN-PLACE]
    │
    ├─ Split: l2 = len/2, m = first + l2
    │
    ├─ CASE A: len ≤ buff_size (buffer large enough)
    │    ├─ __stable_sort_move(first, m, ..., buff)       → sort left half into buff[0..l2)
    │    ├─ __stable_sort_move(m, last, ..., buff+l2)     → sort right half into buff[l2..len)
    │    └─ __merge_move_assign(buff, ..., first)         → merge from buffer back to original
    │
    └─ CASE B: len > buff_size (buffer too small)
         ├─ __stable_sort(first, m, ..., buff, buff_size) → sort left half in-place
         ├─ __stable_sort(m, last, ..., buff, buff_size)  → sort right half in-place
         └─ __inplace_merge(first, m, last, ...)          → merge in-place (fallback)
```

## 5. `__stable_sort_move()` — Sort and Move to Buffer

Source: `original/__algorithm/stable_sort.h:108-154`

This function recursively sorts elements while moving them into the temporary buffer.

```
__stable_sort_move(first1, last1, comp, len, first2):
    ├─ len == 0 → return
    ├─ len == 1 → move-construct single element to buffer
    ├─ len == 2 → compare, move-construct two elements to buffer in order
    ├─ len ≤ 8 → __insertion_sort_move(first1, last1, first2, comp)
    │              (fused insertion sort + move-construct into buffer)
    │
    └─ len > 8:
         ├─ __stable_sort(first1, m, ..., first2, l2)    → sort left half IN-PLACE
         ├─ __stable_sort(m, last1, ..., first2+l2, ...) → sort right half IN-PLACE
         └─ __merge_move_construct(...)                  → merge into buffer
```

> **Crucial Observation:** Top-down recursion **alternates** between `__stable_sort` and `__stable_sort_move`. The algorithm ping-pongs between the original array and the temporary buffer at each level of depth.

## 6. Leaf Node Analysis

Depending on the type, recursion stops at different sizes:

**For trivially copyable types (e.g., `int`):**
- The threshold `__stable_sort_switch::value` is 128.
- `__stable_sort` halts recursion and executes `__insertion_sort` when $N \le 128$.
- The $N \le 8$ threshold in `__stable_sort_move` is **never reached** because chunks are caught by `__stable_sort`'s 128 threshold before they shrink to 8.
- Result: **The effective leaf chunk size is between 64 and 128**, handled entirely by standard insertion sort.

**For non-trivially copyable types:**
- The threshold `__stable_sort_switch::value` is 0.
- `__stable_sort` handles $N=0, 1, 2$ directly, but otherwise splits.
- `__stable_sort_move` handles $N \le 8$ with `__insertion_sort_move` (fused sort and move to buffer).
- Result: **The effective leaf chunk size is $\le 8$**, processed by `__insertion_sort_move` or $N=2$ swaps.

## 7. `std::sort` Small Sorting Networks

While `stable_sort` utilizes `__insertion_sort` at its leaves, `std::sort` (introsort) employs specialized optimization for small arrays. LLVM 15 contains sorting networks (branchless if applicable) for $N=3, 4, 5$:
- `__sort3_maybe_branchless`
- `__sort4_maybe_branchless`
- `__sort5_maybe_branchless`

These rely on `__cond_swap` and are invoked from `__introsort` and `__insertion_sort_incomplete`. `stable_sort`, however, **does not use these**. Instead, it uses loops via `__insertion_sort`.

---

# Integration Strategy for dtsort

The integration plan for LLVM 15 is architecturally identical to LLVM 19, mirroring the exact patch points to ensure maximum efficacy without degrading the tuned paths for larger arrays.

## Patch Point 1: `__stable_sort_move`

For N ≤ `MAX_DT_SIZE`, we can insert `dt_dispatch` to perform an IT-optimal sort in-place, followed by a simple loop to move-construct the elements into the buffer. This intercepts the non-trivial type execution path.

```cpp
// INJECT BEFORE __len <= 8 CHECK
if (__len <= MAX_DT_SIZE) {
    dt_dispatch(__first1, __last1, __comp);
    for (auto __it = __first1; __it != __last1; ++__it, ++__first2)
        ::new ((void*)__first2) value_type(_Ops::__iter_move(__it));
    return;
}
```

## Patch Point 2: `__stable_sort`

For N ≤ `MAX_DT_SIZE`, `dt_dispatch` should be executed to optimally sort in-place. This will cover the trivial type execution paths early in the recursion tree for small N (if $N$ started small) or intercept non-trivial cases. Note that for trivial types, large arrays still fall back to `__insertion_sort` when $64 < N \le 128$.

```cpp
// INJECT BEFORE __len <= __stable_sort_switch::value CHECK
if (__len <= MAX_DT_SIZE) {
    dt_dispatch(__first, __last, __comp);
    return;
}
```

## Patch Point 3: `sort.h` — Introsort Leaf (`std::sort`)

For `std::sort`, LLVM 15 uses `__insertion_sort_3` unconditionally for small partitions (since it lacks the `__leftmost` flag present in newer LLVMs). We intercept this for $N \le \text{MAX\_DT\_SIZE}$.

```cpp
// INJECT BEFORE __len <= __limit CHECK inside __introsort
if (__len <= MAX_DT_SIZE) {
    dt_dispatch(__first, __last, __comp);
    return;
}
```

## Future Improvement: Force Smaller Leaves

Since trivial types (like `int`) terminate recursion early at $N \le 128$, `dtsort` will not be invoked for $N=2..8$ on large datasets because recursion halts long before $N$ reaches `MAX_DT_SIZE`. To fully leverage `dtsort` on large arrays of integers, the 128-element threshold inside `__stable_sort` would either need to be lowered or modified to permit deeper recursion down to $N \le MAX\_DT\_SIZE$. This introduces a trade-off: more merges vs. cheaper leaf sorting.
