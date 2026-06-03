#pragma once

#include "bench_common.hh"
#include "cli.hh"
#include "validation_logic.hh"
#include <iostream>

/**
 * @brief Stage: Consistency Check
 *
 * Verifies that the instrumented metrics (comparisons/moves) are consistent
 * with the algorithm's expectations across different implementations.
 */
template <typename SortImpl>
int run_consistency(const CommandLine &cli, const std::string &variant) {
  int count_iters = 100;
  if (cli.has("count-iters")) {
    count_iters = std::stoi(cli.get("count-iters"));
  }
  if (count_iters == 0) {
    return 0;
  }

  // Default sizes for consistency check
  std::vector<size_t> sizes;
  for (size_t i = 2; i <= MAX_DT_SIZE; ++i)
    sizes.push_back(i);

  // run_large_counts_suite is defined in validation_logic.h
  // It captures counts for IntElement specifically.
  auto results = run_large_counts_suite<SortImpl>("Consistency", sizes, variant,
                                                  count_iters);

  std::cout << "size,avg_cmp,max_cmp,avg_move,max_move\n";
  for (const auto &r : results) {
    std::cout << r.size << "," << r.avg_comparisons << "," << r.max_comparisons
              << "," << r.avg_moves << "," << r.max_moves << "\n";
  }
  return 0;
}
