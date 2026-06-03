#pragma once

#include "bench_common.hh"
#include "cli.hh"
#include "shim_algorithm.hh"
#include "validation_logic.hh"
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Stage: Comparison/Move Counts
 *
 * Measures the algorithmic complexity of the sorting implementation by
 * counting comparisons and moves using instrumented types.
 */
template <typename SortImpl>
int run_counts(const CommandLine &cli, const std::string &impl_name,
               const std::string &variant, bool /*is_stock*/) {
  int iterations = 3;
  if (cli.has("count-iters")) {
    iterations = std::stoi(cli.get("count-iters"));
  }

  auto types = cli.get_list("types");
  std::vector<size_t> sizes;
  std::string sizes_str = cli.get("sizes");
  if (!sizes_str.empty()) {
    auto size_list = cli.get_list("sizes");
    for (const auto &s : size_list)
      sizes.push_back(std::stoull(s));
  } else {
    for (size_t i = 2; i <= MAX_DT_SIZE; ++i)
      sizes.push_back(i);
  }

  auto inputs = cli.get_list("inputs");
  std::vector<InputPattern> patterns;
  if (inputs.empty()) {
    patterns = {InputPattern::Random, InputPattern::ReverseSorted};
  } else {
    for (const auto &s : inputs) {
      if (s == "random") {
        patterns.push_back(InputPattern::Random);
      } else if (s == "reversed" || s == "reverse") {
        patterns.push_back(InputPattern::ReverseSorted);
      } else if (s == "sorted") {
        patterns.push_back(InputPattern::Sorted);
      }
    }
  }

  auto should_run = [&](const std::string &name) {
    if (types.empty())
      return true;
    return shim::contains(types, name);
  };

  std::vector<RunMetrics> all_results;

  auto run_and_append = [&](auto dummy, const std::string &type_name) {
    if (!should_run(type_name))
      return;

    std::vector<size_t> small_sizes, large_sizes;
    for (size_t n : sizes) {
      if (n <= MAX_DT_SIZE)
        small_sizes.push_back(n);
      else
        large_sizes.push_back(n);
    }

    bool is_int = (type_name == "IntNaked" || type_name == "IntElement");

    if (!small_sizes.empty()) {
      for (auto pattern : patterns) {
        auto res = run_large_counts_for_type<decltype(dummy), SortImpl>(
            impl_name, type_name, dummy, small_sizes, variant, iterations,
            pattern);
        all_results.insert(all_results.end(), res.begin(), res.end());
      }
    }

    if (is_int && !large_sizes.empty()) {
      for (auto pattern : patterns) {
        auto res = run_large_counts_for_type<decltype(dummy), SortImpl>(
            impl_name, type_name, dummy, large_sizes, variant, iterations,
            pattern);
        all_results.insert(all_results.end(), res.begin(), res.end());
      }
    }
  };

  run_and_append(IntElement{}, "IntNaked");
  run_and_append(MultiKeyRecord{}, "MultiKey");
  run_and_append(WideRow{}, "WideRow");
  run_and_append(LongStringCommonPrefix{}, "LongPrefix");
  run_and_append(ReversedDomain{}, "RevDomain");
  run_and_append(CaseInsensitiveString{}, "CaseInsens");

  // Print Header for CSV parsing (orchestrator handles duplicates)
  std::cout << "impl,variant,type,size,avg_cmp,max_cmp,avg_move,max_move,avg_"
               "dt_cmp,avg_dt_move,pattern\n";

  for (const auto &m : all_results) {
    std::string pat_name =
        (m.pattern == InputPattern::Sorted          ? "sorted"
         : m.pattern == InputPattern::ReverseSorted ? "reversed"
                                                    : "random");
    std::cout << m.impl_name << "," << m.variant << "," << m.type_name << ","
              << m.size << "," << m.avg_comparisons << "," << m.max_comparisons
              << "," << m.avg_moves << "," << m.max_moves << ","
              << m.avg_dtsort_comparisons << "," << m.avg_dtsort_moves << ","
              << pat_name << "\n";
  }

  return 0;
}
