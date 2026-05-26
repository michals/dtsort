# dtsort: Accelerating C++ Stable Sorting at Scale via Decision-Tree Kernels

**dtsort** is a high-performance C++ template library that generates and implements **highly optimized sorting algorithms** for small, fixed-size arrays. By modeling sorting as a permutation identification problem, it compiles binary decision trees over all possible permutations, minimizing comparisons and applying the inverse permutation via optimal in-place cycle decompositions.

While these sorters are optimized for small arrays, their performance benefits scale up to **arbitrary input sizes** when integrated as the base case inside production-grade hybrid stable sorting algorithms (such as the leaf-level fallbacks in `std::stable_sort` for GCC libstdc++ and LLVM libc++). Combined with advanced adaptation strategies like recursive merge-skipping, `dtsort` significantly accelerates sorting at macroscopic scales by resolving small sub-problems with mathematical precision.

---

## 🚀 Key Features

- **Optimal Comparisons**: Targets the minimal possible number of comparisons for small arrays, aligning with the theoretical limits defined by **[OEIS A036604](https://oeis.org/A036604)**.
- **Zero Extra Memory / In-Place Cycles**: Applies the identified permutation using optimal in-place cycle rotation, requiring at most $k+1$ element moves for a cycle of length $k$.
- **Fully Standard-Compliant C++ API**: Drop-in compatible with standard STL comparison concepts (works with pointers, iterators, and custom functors/lambdas).
- **Consolidated Adaptivity Modes**:
  - **High Entropy Mode (`DT_ADAPTIVE = 0`)**: Optimized for maximum raw speed on completely unsorted/random inputs, minimizing comparisons and branch mispredictions.
  - **Adaptive Mode (`DT_ADAPTIVE = 1`)**: Features run-detection and merge-skip checks to achieve extremely low comparison counts and fast execution on sorted, reversed, or partially sorted inputs.
- **Extensible Architecture**: Structured to support future target languages (e.g., Rust, Go) under a unified codegen engine.

---

## 📂 Repository Structure

The public repository is structured as follows:

```
├── LICENSE                     # MIT License
├── Makefile                    # Convenience entry point (examples, bench, bench-fast)
├── README.md                   # This file
│
├── codegen/                    # C++ Sorter Generator (for transparency and reproducibility)
│   ├── CMakeLists.txt          # Configures generator & verification suites
│   ├── src/                    # Generator source code
│   └── docs/                   # Developer documentation
│
└── cpp/                        # C++ Target Environment
    ├── include/                # Header-only library (main entry point, cycle helpers, and optimal kernels)
    │   ├── dt_stable_sort.hh   # Main entry point (dt_stable_sort)
    │   ├── cycle.hh            # Cycle rotation helper macros
    │   └── dtsort*.hh          # Pre-generated optimal sorting kernels
    ├── examples/               # Build-ready C++ usage examples
    └── benchmark/              # Core performance benchmark suite
        ├── main.py             # Python-based benchmark orchestrator
        └── results/            # Pre-packaged latest benchmark reports & interactive dashboard
```

---

## 🛠️ Getting Started (C++ Integration)

Since `dtsort` is a header-only library under `cpp/include/`, you can simply copy the contents or include the directory in your compiler search path.

To build and run both bundled examples in one step from the repository root:

```bash
make examples
```

### Minimal Example

Here is a clean C++ demo showcasing basic sorting and custom comparators.

```cpp
#include <iostream>
#include <vector>
#include <string>

// Optional: Toggle adaptivity mode before including the library header.
// 0 = High Entropy Mode (pure decision tree, no shortcuts)
// 1 = Adaptive Mode (smart run-skips, default)
#define DT_ADAPTIVE 1

#include "dt_stable_sort.hh"

struct Player {
    std::string name;
    int score;
};

int main() {
    // 1. Sort basic integers
    std::vector<int> data = {7, 2, 8, 3, 1, 6, 5, 4};
    std::cout << "Original ints: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n";

    // Call optimal stable sort (N = 8)
    dt_stable_sort(data.begin(), data.end());

    std::cout << "Sorted ints:   ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n\n";

    // 2. Sort custom objects with a lambda comparator
    std::vector<Player> players = {
        {"Alice", 95}, {"Bob", 80}, {"Charlie", 95}, {"David", 50}
    };

    // Sort players by score descending, preserving order for equal scores (stable)
    dt_stable_sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
        return a.score > b.score;
    });

    std::cout << "Players sorted by score (descending):\n";
    for (const auto& p : players) {
        std::cout << "  " << p.name << ": " << p.score << "\n";
    }

    return 0;
}
```

---

## 📈 Performance & Benchmarking

We provide a comprehensive benchmarking suite inside `cpp/benchmark/` supporting isolated target patching for standard library fallbacks (GCC and LLVM).

To inspect the latest verified speedup curves, hardware configurations, and interactive results, open:
📂 **`cpp/benchmark/results/reports/interactive_report.html`** in any web browser.

---

## 🔁 Reproducing Benchmarks

You can rebuild and re-run the entire benchmark from scratch to verify the results on your own machine.

### Prerequisites

- **CMake 3.16+** and a **C++20-capable compiler** (GCC 10+, Clang 12+, AppleClang 13+, MSVC 19.29+)
- **Python 3.11+** with the [`uv`](https://github.com/astral-sh/uv) package manager
- **Internet connection** — required during the first build to fetch [Google Benchmark](https://github.com/google/benchmark) via CMake `FetchContent`

### Quick Start

From the **repository root**:

```bash
# Run the default benchmark: dt_stable_sort, 50 log-spaced sizes 20..100K
make bench

# Or invoke the orchestrator directly for more control:
cd cpp/benchmark
uv run main.py --impls DT --sizes 50:20:100000 --inputs random,reversed,sorted
```

The orchestrator will automatically:
1. Build the codegen tools and regenerate sorting headers
2. Configure and compile the benchmark binaries (downloads Google Benchmark on first run)
3. Run validation, operation counts, and wall-clock timing
4. Generate an interactive HTML report in `cpp/benchmark/logs/<timestamp>/`

### Runtime Expectations

The default `make bench` scope (50 sizes, 2 input patterns) takes roughly **25–35 minutes** on an Apple M4. A full sweep over all input patterns and types can take **several hours**. Use `--dry-run` to preview timing before committing:

```bash
uv run main.py --impls DT --dry-run
```

For all available options:

```bash
uv run main.py --help
```

### Windows Users

The GCC/LLVM stdlib patching targets require Unix shell scripts and are not available on Windows. The standalone `dt_stable_sort` benchmark (`--impls DT`) works natively on Windows — the build system detects this automatically and skips the Unix-only steps:

```bash
# Windows (PowerShell / Command Prompt)
cd cpp\benchmark
uv run main.py --impls DT --sizes 50:20:100000
```

---

## 📖 Reproducing Sorters

If you want to modify the heuristics, add new sorting variations, or rebuild the decision tree files (`.dt` caches and `.hh` outputs) from scratch, use the generator under `codegen/`:

```bash
cd codegen
cmake -B build && cmake --build build
# Run the generator binary
./build/dtsort-codegen
```

To run the verification test suite and guarantee correctness of the tree logic:

```bash
# Correctness verification under high Entropy and Adaptive modes
./build/dtsort-verify-em --check-all
./build/dtsort-verify-am --check-all
```

---

## 📄 License

All core code and generated artifacts developed specifically for this project are released under the [MIT License](LICENSE).

### Third-Party Code Disclaimer

The benchmarking suite includes snapshots of third-party C++ standard library implementations (located under `cpp/benchmark/cpp/stdlib_impls/` in the public repository) to facilitate safe, reproducible, and isolated patching and comparison:

- GCC standard library (`libstdc++`) snapshots are governed by the **GPL v3 with Runtime Exception**.
- LLVM standard library (`libc++`) snapshots are governed by the **Apache License v2.0 with LLVM Exceptions**.

These third-party source files are **not** covered by the MIT license of this project. They retain their original copyright notices, license headers, and terms, as detailed in the separate license file under [cpp/benchmark/cpp/stdlib_impls/LICENSE](cpp/benchmark/cpp/stdlib_impls/LICENSE).
