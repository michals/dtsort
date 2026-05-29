# dtsort-codegen — Developer Documentation

> **dtsort-codegen** (decision-tree sort generator) produces optimal sorting code for small fixed-size inputs.
> The generated code is published separately as [dtsort](https://github.com/michals/dtsort).

---

## 1. Project Overview

dtsort-codegen is a **code generator** that produces sorting functions (`dtsortN()`) for vectors of size N.

The generated functions:

1. **Identify** the input permutation using an optimal binary decision tree (comparisons only).
2. **Apply** the identified permutation in-place via cycle decomposition (minimal moves, stable).

The generated code (goto + macros) looks unusual but is **machine-generated** — the dtsort-codegen project is the generator.

### Practical Value

Pure dt-sort handles only small N (≤ 8–10), but it is designed to be plugged into **hybrid algorithms** (e.g. top-down recursive merge sort) where the base case is a small sub-array. In benchmarks on Apple M4 hardware for 4KB records (`WideRow`):

| Metric      | vs `std::stable_sort` (N=256) | vs `std::stable_sort` (Large N) |
| ----------- | ----------------------------- | ------------------------------- |
| Wall-clock  | **+55.5%**                    | **+13.7%**                      |
| Comparisons | **+4.5%**                     | **+4.5%**                       |
| Moves       | **+170%** (N=4)               | **+66%** (N=7)                  |

---

## 2. File Map

### Core Algorithm

- [`consts.hh`](../src/consts.hh) — Constants: `N_t`, `MAX_N`, precomputed `FACTORIAL[]`, `VARIATIONS[]`
- [`Digits.hh`](../src/Digits.hh) — `Digits<N>`: represents a sequence of N digits; enumerates permutations and variations; generates **cycle decomposition** strings
- [`Matrix.hh`](../src/Matrix.hh) — `Matrix<N>`: N×N bit-matrix stored as `bitset<N*N>`, base class for comparison matrices
- [`PermMatrix.hh`](../src/PermMatrix.hh) — `PermMatrix<N>`: comparison matrix fully determined by a single permutation
- [`PartialMatrix.hh`](../src/PartialMatrix.hh) — `PartialMatrix<N>`: partially-known comparison matrix with a `mask` bitset tracking known cells
- [`Solutions.hh`](../src/Solutions.hh) — `Solutions<N>`: **core algorithm** — enumerates all permutations, counts variations, implements `split()` for decision tree construction
- [`DecisionTree.hh`](../src/DecisionTree.hh) — `DTNode<N>` + `buildTree<N>()`: recursive decision tree builder (Phase 1)
- [`dtsort_common.hh`](../../cpp/include/dtsort_common.hh) — Centralized macros and `X()` overloads: in-place cycle (right-shift) of 2–10 elements via `std::move`

### Code Generation (`src/gen/`)

- [`Generator.hh`](../src/gen/Generator.hh) — `Generator<N>`: abstract base class for all generators (defines `generate()` interface)
- [`CppGenerator.hh`](../src/gen/CppGenerator.hh) — `CppGenerator<N>`: tree traversal → C++ source code generation (`dtsortN.hh`)
- [`DTGenerator.hh`](../src/gen/DTGenerator.hh) — `DTGenerator<N>`: save/load `.dt` files — allows separating tree construction from code generation (caching)

### Test & Validation

- [`DTValidator.hh`](../src/DTValidator.hh) — `validateTree()`, `validateSorting()`, `validateVariations()`: verifies decision tree correctness, sorting accuracy (including with duplicate elements)
- [`UnitTests.hh`](../src/UnitTests.hh) — `UnitTests` namespace: matrix and partial matrix unit tests
- [`test_main.cc`](../src/test_main.cc) — Entry point for verification binaries (`dtsort-verify`, `dtsort-verify-em`, `dtsort-verify-am`): statistical verification, unit tests, OEIS optimality checks

### Utilities

- [`common.h`](../src/common.h) — Utilities: `ternaryToDec()`, `hexify()`, print operators for vectors/arrays
- [`debug.h`](../src/debug.h) — `Debug()` / `Debugn()` macros (disabled in release via `NDEBUG`)

### Entry Points

- [`main.cc`](../src/main.cc) — `dtsort-codegen` binary: orchestrates generation pipeline (build tree → cache `.dt` → generate `.hh`)

### Output

- `cpp/include/dtsortN_*.hh` — Generated C++ sorting code (e.g. `dtsort8_gs53.hh`, `dtsort5_full.hh`)
- `cpp/output/dtsortN_*.dt` — Serialized decision tree (used for caching; regeneration skipped if present)

### Build & Project Files

- [`CMakeLists.txt`](../CMakeLists.txt) — CMake build config; builds `dtsort-codegen`, `dtsort-verify`, `dtsort-verify-em`, and `dtsort-verify-am` targets

---

## 3. Key Data Structures

### 3.1 `Matrix<N>` — Bit Square Matrix

```
bitset<N*N> data;    // N×N bits stored row-by-row
rc2i(row, col) → index = row*N + col
```

Only the **lower triangle** (row > col) carries meaningful comparison data. The diagonal is always ignored (comparing an element to itself).

### 3.2 `PermMatrix<N>` — Permutation Comparison Matrix

For a permutation `p = (p₀, p₁, ..., pₙ₋₁)`:

```
M[row][col] = (p[row] < p[col])    for row > col
```

This encodes all pairwise comparison outcomes.

### 3.3 `PartialMatrix<N>` — Partially Known Comparison State

```
bitset<N*N> data;    // comparison values (0/1)
bitset<N*N> mask;    // which cells are known (1) vs unknown (0)
```

Unknown cells are displayed as `_`.

### 3.4 `Digits<N>` — Permutation / Variation Representation

```
array<uint8_t, N> data;
```

Handles permutation enumeration and cycle decomposition for `X()` calls.

---

## 4. Two-Phase Pipeline

The generation runs in two phases:

1. **Decision tree construction** (`Solutions<N>` + `buildTree<N>()`) — builds the optimal binary decision tree that identifies any permutation from its comparisons. Trees are cached as `.dt` files via `DTGenerator`.
2. **Code generation** (`CppGenerator<N>`) — traverses the tree and emits C++ source code using `goto`-based labels and `Q()`/`X()` macros.

---

## 5. Building and Running

```bash
# Build all binaries from the codegen directory
cmake -B build -S . && cmake --build build

# Or build from the top-level repository root using make
make build

# Generate all sorters (cpp/include/dtsort*.hh and cpp/output/dtsort*.dt)
./build/dtsort-codegen

# Generate only N=5
./build/dtsort-codegen -n 5

# Run fast verification in High Entropy Mode (permutation checks only, N=2..8)
./build/dtsort-verify-em --check-all

# Run fast verification in Adaptive Mode
./build/dtsort-verify-am --check-all

# Run with variation checks (slower, tests N^N sequences including duplicates)
./build/dtsort-verify-am --check-all --check-variations

# Run unit tests
./build/dtsort-verify-em --unit-tests

# Or use CTest
cmake -B build -S . && cd build && ctest -L fast               # fast permutation checks
cmake -B build -S . && cd build && ctest -L full               # full checks including variations
cmake -B build -S . && cd build && ctest -L unit               # unit tests
```

---

## 6. Adaptivity Configuration

The project supports two main compile-time sorting configurations, controlled by the `DT_ADAPTIVE` boolean macro inside `dt_stable_sort.hh`:

1. **High Entropy Mode** (`DT_ADAPTIVE=0`):
   - Optimized for completely random or high-entropy data where pre-sorted or partially-sorted sub-arrays are extremely rare.
   - All small sorting kernels return `void` (no sorting status tracking).
   - Early-exit and short-circuit optimizations are completely disabled, reducing comparison and branching overhead on random inputs.

2. **Adaptive Mode** (`DT_ADAPTIVE=1`, default):
   - Designed to handle partially-sorted, reversed, or duplicate-heavy data distributions efficiently.
   - Small sorting kernels return `bool` indicating whether any work was actually done (i.e. `false` if the input was already sorted).
   - In composite/group-sorted kernels (e.g. `gs55`), if all sub-groups report no work and their boundary elements are in-order, the remaining merges are short-circuited entirely.
   - In recursive sorting merges (`dt_stable_sort`), a merge-skip comparison (`*(mid-1) <= *mid`) is performed before calling `std::inplace_merge`, avoiding merge scans when the sub-arrays are already in order.

---
