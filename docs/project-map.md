# dtsort — Repository Map

```
dtsort/
├── cpp/
│   ├── include/                ← Header-only library (this is the main deliverable)
│   │   ├── dt_stable_sort.hh   ← Drop-in replacement for std::stable_sort
│   │   ├── dtsort_common.hh    ← Centralized macros and cycle rotation helpers
│   │   └── dtsort*.hh          ← Generated optimal sorting kernels (N=2..10)
│   ├── examples/               ← Build-ready C++ usage examples
│   │   ├── simple_sort.cc      ← Basic sorting + custom comparators demo
│   │   └── simple_benchmark.cc ← Side-by-side timing vs std::stable_sort
│   └── benchmark/              ← Full performance benchmark suite
│       ├── main.py             ← Python-based benchmark orchestrator
│       ├── runner/             ← Benchmark Python modules
│       ├── cpp/                ← C++ benchmark binaries & stdlib patches
│       ├── templates/          ← HTML dashboard template for report generation
│       └── results/            ← Pre-packaged benchmark results (see below)
├── codegen/                    ← C++ decision tree code generator
│   ├── src/                    ← Generator source code
│   │   ├── main.cc             ← Generator binary (dtsort-codegen)
│   │   ├── test_main.cc        ← Verification binary (dtsort-verify)
│   │   ├── gen/                ← Code generators (CppGenerator, DTGenerator)
│   │   ├── Solutions.hh        ← Core: permutation & split evaluation
│   │   ├── DecisionTree.hh     ← Recursive tree builder
│   │   └── DTValidator.hh      ← Tree correctness & variation validator
│   ├── docs/
│   │   ├── history.md          ← The 20-year journey from student project to dtsort
│   │   └── developer_guide.md  ← Data structures, algorithms, file maps
│   └── CMakeLists.txt          ← Generator build configuration
├── Makefile                    ← Quick-start entry point (examples, bench, bench-fast)
├── RESULTS.md                  ← Benchmark results walkthrough with analysis
├── LICENSE                     ← MIT License
└── README.md                   ← Main project readme
```

## Pre-Packaged Benchmark Results

The `cpp/benchmark/results/` directory contains verified benchmark data from a complete run on Apple M4 hardware. The configuration used to produce these results is stored in `cpp/benchmark/results/run_config.json`.

The benchmark was generated with:

```bash
uv run main.py -n 10 --impls DT,GCC16,LLVM19 --inputs random,reversed,sorted \
    --adaptivity adaptive,entropy --sizes 100:20:2000000
```

### Reports

The `reports/` subdirectory contains:

| File | Description |
|---|---|
| `interactive_report.html` | Standalone interactive dashboard (open in any browser). This is the same content published at [michals.github.io/dtsort/interactive_report.html](https://michals.github.io/dtsort/interactive_report.html). |
| `fig1_speedup_grid.svg` | Multi-panel speedup curves across all implementations |
| `fig2_leaf_throughput.svg` | Small-N (2..10) throughput comparison bars |
| `fig3_space_complexity.svg` | Generated code size breakdown per sorter |
| `executive_summary.md` | Peak speedup cheat sheet and sorter characteristics |

### Reproducing

To reproduce these results on your own hardware:

```bash
make bench   # Several hours on Apple M4 (full 100-size sweep)
```

See the main [README](../README.md) for prerequisites and detailed instructions.

## Key Source Files

| File | What it does |
|---|---|
| `cpp/include/dt_stable_sort.hh` | The main entry point. Includes all generated kernels, implements the hybrid merge-sort engine with DFS ping-pong recursion, block-scoped index sorting, and adaptive merge-skipping. |
| `cpp/include/dtsort_common.hh` | Centralized macros and move-optimal cycle rotation templates (`X()` functions). |
| `cpp/include/dtsort10_gs55.hh` | The flagship N=10 kernel using Group-Sorted (gs55) architecture. Due to `MAX_N=10` alignment, the recursive split ensures the majority of leaf blocks are handled by this kernel. |
| `codegen/src/Solutions.hh` | The core algorithm: comparison matrix evaluation and greedy entropy-based split selection. |
| `codegen/src/DecisionTree.hh` | Recursive decision tree builder (`buildTree<N>`). |
| `codegen/src/gen/CppGenerator.hh` | Transforms decision trees into C++ `goto`-based sorting functions. |
