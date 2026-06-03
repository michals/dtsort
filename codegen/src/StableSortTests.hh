#pragma once

#include "dt_stable_sort.hh"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

namespace StableSortTests {

/**
 * @brief Helper structure to verify stability by tracking original indices.
 */
struct Element {
  int value;
  int original_index;

  bool operator<(const Element &other) const { return value < other.value; }
};

/**
 * @brief Verifies that equal elements preserve their relative original order.
 */
inline void verify_stability() {
  std::cout << "Verifying Stability... ";

  for (int N : {2, 3, 4, 5, 7, 8, 13, 16, 27, 32, 64, 100}) {
    std::vector<Element> data(N);
    // Create patterns with duplicates
    for (int i = 0; i < N; ++i) {
      data[i] = {i / 2, i}; // duplicates every 2 elements
    }

    // Shuffle to make it interesting
    std::shuffle(data.begin(), data.end(), std::mt19937(42));

    // Re-assign original indices to track them
    for (int i = 0; i < N; ++i) {
      data[i].original_index = i;
    }

    dt_stable_sort(data.begin(), data.end());

    // Verify sorted and stable
    for (size_t i = 1; i < data.size(); ++i) {
      if (data[i].value < data[i - 1].value) {
        std::cout << "FAILED (Not sorted at N=" << N << ")\n";
        exit(1);
      }
      if (data[i].value == data[i - 1].value) {
        if (data[i].original_index < data[i - 1].original_index) {
          std::cout << "FAILED (Not stable at N=" << N << ")\n";
          exit(1);
        }
      }
    }
  }
  std::cout << "OK\n";
}

/**
 * @brief Smoke tests for large array sizes with random, sorted, and reversed
 * inputs.
 */
inline void verify_large_n() {
  std::cout << "Verifying Large N (Correctness)... ";

  std::mt19937 gen(1337);
  for (int N : {100, 1000, 10000, 100000}) {
    std::vector<int> data(N);

    // Random
    std::generate(data.begin(), data.end(), [&]() { return gen() % 1000; });
    dt_stable_sort(data.begin(), data.end());
    if (!std::is_sorted(data.begin(), data.end())) {
      std::cout << "FAILED (Random N=" << N << ")\n";
      exit(1);
    }

    // Reversed
    std::iota(data.rbegin(), data.rend(), 0);
    dt_stable_sort(data.begin(), data.end());
    if (!std::is_sorted(data.begin(), data.end())) {
      std::cout << "FAILED (Reversed N=" << N << ")\n";
      exit(1);
    }

    // Sorted
    std::iota(data.begin(), data.end(), 0);
    dt_stable_sort(data.begin(), data.end());
    if (!std::is_sorted(data.begin(), data.end())) {
      std::cout << "FAILED (Sorted N=" << N << ")\n";
      exit(1);
    }
  }
  std::cout << "OK\n";
}

/**
 * @brief Validates edge cases such as empty ranges and single-element ranges.
 */
inline void verify_edge_cases() {
  std::cout << "Verifying Edge Cases... ";

  // N=0
  std::vector<int> v0;
  dt_stable_sort(v0.begin(), v0.end());

  // N=1
  std::vector<int> v1 = {42};
  dt_stable_sort(v1.begin(), v1.end());
  if (v1[0] != 42) {
    std::cout << "FAILED (N=1)\n";
    exit(1);
  }

  // N=2
  std::vector<int> v2 = {2, 1};
  dt_stable_sort(v2.begin(), v2.end());
  if (v2[0] != 1 || v2[1] != 2) {
    std::cout << "FAILED (N=2)\n";
    exit(1);
  }

  std::cout << "OK\n";
}

/**
 * @brief Entry point for the dt_stable_sort verification suite.
 */
inline void run_all() {
  std::cout << "--- Running dt_stable_sort tests ---\n";
  verify_edge_cases();
  verify_stability();
  verify_large_n();
  std::cout << "All dt_stable_sort tests PASSED.\n";
}

} // namespace StableSortTests
