#include "cli.hh"
#include <iostream>
#include <string>
#include <vector>

// Implementation-specific headers
#if defined(IMPL_GCC14) || defined(IMPL_GCC16) || defined(IMPL_GCC7)
#include "stl_algo.h"
#elif defined(IMPL_LLVM15)
#include "llvm15_stable_sort.h"
#elif defined(IMPL_LLVM19)
#include "llvm19_algorithm/stable_sort.h"
#elif defined(IMPL_DT_STABLE_SORT)
#include "dt_stable_sort.hh"
#else
#include <algorithm>
#endif

#include "modes/benchmark.hh"
#include "modes/consistency.hh"
#include "modes/counts.hh"
#include "modes/validate.hh"
#include "modes/weight.hh"

// The patched headers define everything in target_std namespace
// to avoid conflicts with system std headers.

struct SortImpl {
  template <typename Iter, typename Cmp>
  static void stable_sort(Iter first, Iter last, Cmp cmp) {
#if defined(IMPL_GCC14) || defined(IMPL_GCC16) || defined(IMPL_GCC7) ||        \
    defined(IMPL_LLVM15) || defined(IMPL_LLVM19)
    target_std::stable_sort(first, last, cmp);
#elif defined(IMPL_DT_STABLE_SORT)
#if IS_STOCK
    std::stable_sort(first, last, cmp);
#else
    ::dt_stable_sort(first, last, cmp);
#endif
#else
    std::stable_sort(first, last, cmp);
#endif
  }

  template <typename Iter> static void stable_sort(Iter first, Iter last) {
#if defined(IMPL_GCC14) || defined(IMPL_GCC16) || defined(IMPL_GCC7) ||        \
    defined(IMPL_LLVM15) || defined(IMPL_LLVM19)
    target_std::stable_sort(first, last);
#elif defined(IMPL_DT_STABLE_SORT)
#if IS_STOCK
    std::stable_sort(first, last);
#else
    ::dt_stable_sort(first, last);
#endif
#else
    std::stable_sort(first, last);
#endif
  }
};

void print_help(const char *prog) {
  std::cout << "Usage: " << prog << " --stage=[STAGE] [OPTIONS]\n"
            << "Stages:\n"
            << "  validate    - Run correctness tests\n"
            << "  counts      - Measure comparison/move counts\n"
            << "  consistency - Verify metrics consistency for small N\n"
            << "  weight      - Measure data type weights (ns/op)\n"
            << "  benchmark   - Run wall-clock benchmarks (Google Benchmark)\n"
            << "  config      - Output compiled parameters\n\n"
            << "Options:\n"
            << "  --types=T1,T2    - Comma-separated data types\n"
            << "  --sizes=S1,S2    - Comma-separated dataset sizes\n"
            << "  --count-iters=N  - Iterations for counts stage\n"
            << "  --bench-iters=N  - Repetitions for benchmark stage\n"
            << "  --help           - Show this help\n";
}

int main(int argc, char **argv) {
  CommandLine cli(argc, argv);

  if (cli.has("help") || argc < 2) {
    print_help(argv[0]);
    return 0;
  }

  std::string stage = cli.get("stage");
  if (stage.empty() && !cli.positional().empty()) {
    stage = cli.positional()[0];
  }
  std::string impl_name = "Unknown";
#ifdef IMPL_NAME
  impl_name = IMPL_NAME;
#endif

  bool is_stock = false;
#if IS_STOCK
  is_stock = true;
#endif

  std::string variant = is_stock ? "stock" : "dtsort";
#ifdef VARIANT
  variant = VARIANT;
#endif

  if (stage == "validate") {
    return run_validate<SortImpl>(cli, impl_name, variant);
  } else if (stage == "counts") {
    return run_counts<SortImpl>(cli, impl_name, variant, is_stock);
  } else if (stage == "consistency") {
    return run_consistency<SortImpl>(cli, variant);
  } else if (stage == "weight") {
    return run_weight(cli);
  } else if (stage == "benchmark") {
    return run_benchmark<SortImpl>(argc, argv, cli, impl_name, variant);
  } else if (stage == "config") {
    std::cout << "MAX_DT_SIZE=" << MAX_DT_SIZE << "\n";
#ifdef DT_BLOCK_SIZE
    std::cout << "DT_BLOCK_SIZE=" << DT_BLOCK_SIZE << "\n";
#endif
    std::cout << "DT_MODE=GS\n";
    return 0;
  } else {
    std::cerr << "Error: Unknown stage '" << stage << "'\n";
    print_help(argv[0]);
    return 1;
  }
}
