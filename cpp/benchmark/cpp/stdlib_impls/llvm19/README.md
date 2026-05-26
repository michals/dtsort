# LLVM 19 stable_sort Extraction

This directory contains the extracted `stable_sort` implementation from LLVM 19 libc++.

## Source Origin
- **Source**: LLVM 19.x release headers (`__algorithm`, `__functional`, `__bit`, etc.)

## License
The files in `original/` are snapshots of the LLVM standard library (libc++) and are covered by the **Apache License v2.0 with LLVM Exceptions**.

## Structure

- `original/`: Original headers downloaded from LLVM 19.x release.
- `generated/`: Patched headers for use with `dtsort`.
- `generated_stock/`: Patched headers with stock algorithms (no `dtsort`).
- `adapter.h`: Adapter for patched version (defines required `_LLVM19_` macros and shims).
- `patches/`: Contains the `dtsort_logic.patch` for algorithmic changes.
- `patch_dtsort.sh`: Script to generate `generated/` from `original/`.
- `patch_stock.sh`: Script to generate `generated_stock/` from `original/`.

## Generation

To regenerate the headers:

```bash
./patch_dtsort.sh    # generates generated/ (with dtsort)
./patch_stock.sh     # generates generated_stock/ (stock algorithms)
```

## Integration Details

LLVM 19 has an extremely modular header architecture. `patch_dtsort.sh` creates hundreds of stub files mapped to `llvm19_*` directories to satisfy the deep dependency tree without pulling in system-level `<__type_traits>` or `<__iterator>` files.

---

# Algorithm Deep-Dive: `std::stable_sort()` in LLVM 19

## 1. High-Level Strategy

LLVM 19's `stable_sort` implements a **top-down recursive merge sort** with a temporary buffer. The algorithm has two modes determined by whether a sufficiently large buffer could be allocated:

| Condition | Strategy | Complexity |
|-----------|----------|------------|
| `buffer ≥ len` | **Buffered merge sort** — sort-and-move into buffer, merge back | O(N log N) guaranteed |
| `buffer < len` | **In-place merge sort** — recursive split + `inplace_merge` | O(N log²N) worst case |

In practice, the buffer allocation almost always succeeds (it requests `N` elements via `get_temporary_buffer`), so the fast buffered path dominates.

## 2. Entry Point Chain

```
stable_sort(first, last, comp)
  └─ __stable_sort_impl<_ClassicAlgPolicy>(first, last, comp)
       ├─ Compute len = last - first
       ├─ If len > __stable_sort_switch::value → allocate buffer of size len
       └─ __stable_sort(first, last, comp, len, buff, buff_size)
```

## 3. The `__stable_sort_switch` Threshold

```cpp
template <class _Tp>
struct __stable_sort_switch {
    static const unsigned value = 128 * is_trivially_copy_assignable<_Tp>::value;
};
```

| Type Category | Threshold | Effect |
|---------------|-----------|--------|
| Trivially copy-assignable (`int`, `double`, POD structs) | **128** | Uses insertion sort for ≤128 elements; buffer allocated only for >128 |
| Non-trivially copy-assignable (`std::string`, custom types) | **0** | Buffer always allocated; insertion sort fallback in `__stable_sort` is never taken |

## 4. `__stable_sort()` — Main Recursive Function

Source: [`original/__algorithm/stable_sort.h:192-235`](original/__algorithm/stable_sort.h)

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
         └─ __inplace_merge(first, m, last, ...)          → merge in-place (slow path)
```

## 5. `__stable_sort_move()` — Sort and Move to Buffer

Source: [`original/__algorithm/stable_sort.h:144-185`](original/__algorithm/stable_sort.h)

```
__stable_sort_move(first1, last1, comp, len, first2):
    ├─ len == 0 → return
    ├─ len == 1 → move-construct single element to buffer
    ├─ len == 2 → compare, move-construct two elements to buffer in order
    ├─ len ≤ 8 → __insertion_sort_move(first1, last1, first2, comp)
    │              (insertion sort that move-constructs into buffer)
    │
    └─ len > 8:
         ├─ __stable_sort(first1, m, ..., first2, l2)    → sort left half IN-PLACE
         ├─ __stable_sort(m, last1, ..., first2+l2, ...) → sort right half IN-PLACE
         └─ __merge_move_construct(...)                  → merge into buffer
```

> **Key**: The recursion **alternates** between `__stable_sort` and `__stable_sort_move`, creating a ping-pong pattern where data moves between the original array and the buffer.

## 6. Critical: Which Base Case Is Actually Hit?

The alternating recursion means the **effective leaf node depends on type triviality**:

**For trivially copyable types (`int`, `double`, POD):**
- `__stable_sort` catches at N ≤ 128 → `__insertion_sort`
- `__stable_sort_move(N ≤ 8)` is **NEVER REACHED** because `__stable_sort` intercepts first
- Typical leaf chunk size: ~60-128 elements

**For non-trivially copyable types (`std::string`):**
- `__stable_sort_switch::value = 0` → `__stable_sort` never uses insertion sort
- `__stable_sort_move(N ≤ 8)` IS the leaf → `__insertion_sort_move`
- `__stable_sort` only handles N=0,1,2 explicitly

This is the most important insight for dtsort integration strategy.

## 7. Helper Functions

| Function | Purpose |
|----------|---------|
| `__insertion_sort_move` | **Fused** sort + move-construct to buffer. Used by `__stable_sort_move` for N ≤ 8 |
| `__merge_move_construct` | Merge two sorted ranges → uninitialized buffer (placement new) |
| `__merge_move_assign` | Merge two sorted ranges → initialized output (move-assign) |
| `__insertion_sort` | Standard in-place insertion sort. Used by `__stable_sort` for N ≤ 128 |
| `__inplace_merge` | In-place merge: buffered if half fits, else recursive rotate-based |
| `__buffered_inplace_merge` | Copy smaller half to buffer, merge |

## 8. Buffer Management

- `get_temporary_buffer<T>(N)`: tries to allocate N×sizeof(T) bytes; halves and retries on failure
- For trivially copyable types with N ≤ 128: **no buffer allocated**
- For non-trivial types: **buffer always requested** (threshold is 0)
- Buffer freed via `__return_temporary_buffer` (operator delete)

---

# dtsort Integration

## Current Patch (v2) — Patch Points

The `dtsort_logic.patch` modifies three locations. The key design principle is:
**dtsort handles N ≤ MAX_DT_SIZE; stock algorithms handle the rest.**

### Patch Point 1: `__stable_sort_move` — Sort and Move to Buffer

```cpp
// NEW: dtsort for N ≤ MAX_DT_SIZE (sort in-place, then move to buffer)
if (__len <= MAX_DT_SIZE) {
    dt_dispatch(__first1, __last1, __comp);
    // move-construct sorted elements to buffer
    for (auto __it = __first1; __it != __last1; ++__it, ++__first2)
        ::new ((void*)__first2) value_type(_Ops::__iter_move(__it));
    return;
}
// STOCK: fused insertion sort + move for MAX_DT_SIZE < N ≤ 8
if (__len <= 8) {
    __insertion_sort_move(__first1, __last1, __first2, __comp);
    return;
}
```

**Rationale**: Only use dtsort for sizes it's optimized for. The stock `__insertion_sort_move` is a fused sort+move operation that avoids the extra N moves; we keep it for sizes above MAX_DT_SIZE.

**Note**: For trivially copyable types, this code path is effectively unreachable (see §6).

### Patch Point 2: `__stable_sort` — In-Place Sorting

```cpp
// NEW: dtsort for N ≤ MAX_DT_SIZE (IT-optimal comparisons)
if (__len <= MAX_DT_SIZE) {
    dt_dispatch(__first, __last, __comp);
    return;
}
// STOCK: insertion sort for MAX_DT_SIZE < N ≤ 128 (trivial types)
if (__len <= __stable_sort_switch<value_type>::value) {
    __insertion_sort(__first, __last, __comp);
    return;
}
```

**Rationale**: dtsort provides IT-optimal comparisons for small N. For larger chunks (9-128 for trivial types), LLVM's insertion sort is well-tuned and should not be replaced with dt_dispatch's fallback.

**For non-trivial types**: The `__stable_sort_switch::value` is 0, so the second branch is never taken. The dtsort branch short-circuits recursive merge sort for small N — a real improvement over stock which would recurse all the way down to N=1,2 base cases.

### Patch Point 3: `sort.h` — Introsort Leaf (std::sort)

```cpp
// In __introsort, for leftmost partitions with len < 24:
if (__leftmost) {
    dt_dispatch(__first, __last, __comp);  // replaces __insertion_sort
}
```

**Note**: The non-leftmost path (`__insertion_sort_unguarded`) is intentionally NOT patched — it exploits a sentinel property that dtsort cannot leverage.

## Coverage Analysis

### For trivially copyable types (primary benchmark case):

| Chunk Size | stable_sort Path | Handler |
|------------|-----------------|---------|
| N = 0, 1 | `__stable_sort` base case | No-op |
| N = 2 | `__stable_sort` base case | Compare + swap |
| N = 3..MAX_DT_SIZE | `__stable_sort` | **dtsort** ✓ |
| N = MAX_DT_SIZE+1..128 | `__stable_sort` | Stock `__insertion_sort` |
| N > 128 | Recursive merge sort | Split + merge |

**Typical leaf size**: ~60-128 elements → handled by stock insertion sort, NOT dtsort.

### For non-trivially copyable types:

| Chunk Size | stable_sort Path | Handler |
|------------|-----------------|---------|
| N = 0, 1, 2 | `__stable_sort` / `__stable_sort_move` base cases | No-op / move |
| N = 3..MAX_DT_SIZE | `__stable_sort` / `__stable_sort_move` | **dtsort** ✓ |
| N = MAX_DT_SIZE+1..8 | `__stable_sort_move` | Stock `__insertion_sort_move` |
| N > 8 | Recursive merge sort | Split + merge |

---

# Future Optimization Opportunities

## Lowering the Recursion Threshold (Potential High Impact)

For trivially copyable types, the leaf chunks are ~60-128 elements — far above MAX_DT_SIZE. This means **dtsort has minimal coverage in `stable_sort` for the common case**.

To increase dtsort coverage, one could lower the `__stable_sort_switch` threshold:

```cpp
// AGGRESSIVE: force merge sort down to MAX_DT_SIZE leaves
if (__len <= MAX_DT_SIZE) {
    dt_dispatch(__first, __last, __comp);
    return;
}
// Remove or lower the 128-element insertion sort threshold
```

**Tradeoff**: More merge operations (deeper recursion) in exchange for IT-optimal comparisons at every leaf. This needs careful benchmarking — the merge overhead may negate the comparison savings.

## Fused `dtsort_move` Variant

The `__stable_sort_move` path requires sort + move-to-buffer. The current approach sorts in-place then copies (N extra moves). A generator-level change could produce `dtsortN_move()` variants that place elements directly into the buffer during cycle decomposition, eliminating the extra copy.

## `std::sort` — Unguarded Insertion Sort Path

The non-leftmost partition path in `__introsort` uses `__insertion_sort_unguarded` (no bounds check on inner loop). A hybrid approach could be: dtsort for N ≤ MAX_DT_SIZE, then `__insertion_sort_unguarded` for the rest of N < 24. This would require splitting the dispatch similar to how we split `__stable_sort`.

## `std::sort` — Branchless Sorting Networks

LLVM 19 uses branchless `__cond_swap`-based sorting networks for `__sort3/4/5` on arithmetic types with simple comparators. These avoid branch misprediction. dtsort uses branching decision trees which may be slower for these specific cases. A potential optimization: skip dtsort for N ≤ 5 when the branchless path would apply.

---

# Reference

## Function Map

| Function | File | Purpose |
|----------|------|---------|
| `stable_sort` | `stable_sort.h:258` | Public API entry point |
| `__stable_sort_impl` | `stable_sort.h:237` | Buffer allocation + dispatch |
| `__stable_sort` | `stable_sort.h:192` | Main recursive merge sort |
| `__stable_sort_move` | `stable_sort.h:144` | Sort + move-construct to buffer |
| `__insertion_sort_move` | `stable_sort.h:37` | Fused insertion sort → buffer |
| `__merge_move_construct` | `stable_sort.h:70` | Merge two sorted ranges → buffer |
| `__merge_move_assign` | `stable_sort.h:108` | Merge two sorted ranges → output |
| `__stable_sort_switch` | `stable_sort.h:187` | Compile-time threshold (128 or 0) |
| `__insertion_sort` | `sort.h:282` | Standard in-place insertion sort |
| `__insertion_sort_unguarded` | `sort.h:312` | Unguarded insertion sort |
| `__sort3` / `__sort4` / `__sort5` | `sort.h:49/87/105` | Small sorting networks (branching) |
| `__sort3_maybe_branchless` | `sort.h:185` | Branchless sort-3 for arithmetic types |
| `__introsort` | `sort.h:756` | Main introsort loop (`std::sort`) |
| `__inplace_merge` | `inplace_merge.h:125` | In-place merge (buffered or recursive) |

## Comparison: `stable_sort` vs `sort`

| Aspect | `stable_sort` | `sort` (introsort) |
|--------|---------------|---------------------|
| Algorithm | Top-down merge sort | pdqsort variant |
| Stability | ✅ Guaranteed | ❌ Not guaranteed |
| Extra space | O(N) buffer | O(1) in-place |
| Small-N leaf | Insertion sort (≤128 trivial, ≤8 in move path) | Sorting networks 3/4/5, insertion sort 6-23 |
| Branchless | ❌ Never | ✅ For arithmetic types |
| Worst case | O(N log N) with buffer, O(N log²N) without | O(N log N) (heap sort fallback) |
