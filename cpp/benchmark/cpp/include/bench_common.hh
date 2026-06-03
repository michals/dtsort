#pragma once
#include "data_generators.hh"
#include <string>

// Structure to hold metrics for a single run
struct RunMetrics {
  std::string impl_name;
  std::string type_name;
  size_t size;
  double avg_comparisons;
  size_t max_comparisons;
  double avg_dtsort_comparisons;
  double avg_moves;
  size_t max_moves;
  double avg_dtsort_moves;
  size_t copies;
  std::string variant;
  InputPattern pattern;
  bool is_stock; // true if stock implementation, false if patched
};
