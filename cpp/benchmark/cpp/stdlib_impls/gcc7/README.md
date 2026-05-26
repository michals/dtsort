# GCC 7 std::stable_sort Extraction

This directory contains an extracted and patched version of `std::stable_sort` from GCC 7.5.0 (libstdc++-v3).

## Source Origin

- **File**: `libstdc++-v3/include/bits/stl_algo.h`
- **Version**: GCC 7.5.0
- **URL**: https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-7.5.0/libstdc++-v3/include/bits/stl_algo.h
- **Dependency**: `bits/predefined_ops.h` (Same version)

## License

The files in `original/` are snapshots of the GCC standard library and are covered by the **GPLv3 with GCC Runtime Library Exception**.
We include unmodified copies here for reproducibility of benchmark results.
Modifications in `generated/` are derived from these snapshots.

## Structure

- `original/`: Unmodified files downloaded from the URLs above.
- `generated/`: Files processed by `patch_dtsort.sh` to:
  - Keep `namespace std` but internally alias it to `target_std` to avoid linker conflicts when testing.
  - Integrate `dt_dispatch` for small array sorting by intercepting `__insertion_sort`.
- `patches/`: Contains the `dtsort_logic.patch` for algorithmic changes.
- `patch_dtsort.sh`: Script to generate `generated/` from `original/`.
- `adapter.h`: Compatibility shims (macros, missing types like `_Temporary_buffer`) to allow compiling GCC headers with other compilers (Clang, newer GCC).
  - Also defines `__insertion_sort_stock` to map back to the original implementation for the split-dispatch strategy.

## Benchmarking Strategy

In our separate-binaries architecture, this patched implementation is built as an independent executable (`bench-gcc7-dtsort`), ensuring isolated include paths without complex namespace renaming. The `stock` version simply uses the `original/` headers.
