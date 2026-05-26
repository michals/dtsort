#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <iomanip>

// Define adaptivity configuration if not already set:
//   0 = High Entropy Mode (optimal for fully random inputs, pure decision tree)
//   1 = Adaptive Mode (optimal for sorted, reversed, or runs, standard default)
#ifndef DT_ADAPTIVE
#define DT_ADAPTIVE 1
#endif

// Include the dtsort interface
#include "dt_stable_sort.hh"

// A simple structure to represent objects being sorted
struct Student {
  int id;
  int grade;
  std::string name;

  // Custom stable comparator: sorts primarily by grade (ascending).
  // If grades are equal, stability should preserve the original order (or we can verify it).
  bool operator<(const Student &other) const {
    return grade < other.grade;
  }
};

int main() {
  std::cout << "==================================================\n";
  std::cout << "     dtsort vs std::stable_sort Benchmark\n";
  std::cout << "     Adaptivity Mode (DT_ADAPTIVE): " << DT_ADAPTIVE << "\n";
  std::cout << "==================================================\n\n";

  constexpr int NUM_ELEMENTS = 500;
  constexpr int NUM_RUNS = 1000;

  std::cout << "Generating " << NUM_ELEMENTS << " random Student objects...\n";
  
  // Set up reproducible random generator
  std::mt19937 rng(1337); 
  std::uniform_int_distribution<int> grade_dist(0, 100);
  
  std::vector<Student> base_data;
  base_data.reserve(NUM_ELEMENTS);
  for (int i = 0; i < NUM_ELEMENTS; ++i) {
    base_data.push_back({
      i,                             // original index (for stability tracking/ID)
      grade_dist(rng),               // random grade between 0 and 100
      "Student_" + std::to_string(i) // name payload
    });
  }

  std::cout << "Preparing " << NUM_RUNS << " copies for std::stable_sort...\n";
  std::vector<std::vector<Student>> std_copies(NUM_RUNS, base_data);

  std::cout << "Preparing " << NUM_RUNS << " copies for dt_stable_sort...\n";
  std::vector<std::vector<Student>> dt_copies(NUM_RUNS, base_data);

  std::cout << "Running benchmark...\n\n";

  // -------------------------------------------------------------------------
  // Benchmark 1: std::stable_sort
  // -------------------------------------------------------------------------
  std::cout << "  [1/2] Benchmarking std::stable_sort()... " << std::flush;
  auto t_start_std = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_RUNS; ++i) {
    std::stable_sort(std_copies[i].begin(), std_copies[i].end());
  }
  auto t_end_std = std::chrono::high_resolution_clock::now();
  auto dur_std = std::chrono::duration_cast<std::chrono::microseconds>(t_end_std - t_start_std).count();
  std::cout << "Done.\n";

  // -------------------------------------------------------------------------
  // Benchmark 2: dt_stable_sort
  // -------------------------------------------------------------------------
  std::cout << "  [2/2] Benchmarking dt_stable_sort()...  " << std::flush;
  auto t_start_dt = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < NUM_RUNS; ++i) {
    dt_stable_sort(dt_copies[i].begin(), dt_copies[i].end());
  }
  auto t_end_dt = std::chrono::high_resolution_clock::now();
  auto dur_dt = std::chrono::duration_cast<std::chrono::microseconds>(t_end_dt - t_start_dt).count();
  std::cout << "Done.\n\n";

  // -------------------------------------------------------------------------
  // Correctness and Stability Verification
  // -------------------------------------------------------------------------
  std::cout << "Verifying correctness and stability... " << std::flush;
  bool passed = true;
  for (int i = 0; i < NUM_RUNS; ++i) {
    // 1. Must be sorted
    if (!std::is_sorted(dt_copies[i].begin(), dt_copies[i].end())) {
      passed = false;
      break;
    }
    // 2. Must match std::stable_sort exactly (validates stability since std::stable_sort is stable)
    for (size_t j = 0; j < base_data.size(); ++j) {
      if (dt_copies[i][j].id != std_copies[i][j].id || 
          dt_copies[i][j].grade != std_copies[i][j].grade ||
          dt_copies[i][j].name != std_copies[i][j].name) {
        passed = false;
        break;
      }
    }
  }
  
  if (passed) {
    std::cout << "SUCCESS! (All runs matched std::stable_sort exactly)\n\n";
  } else {
    std::cout << "FAILED! (Results do not match std::stable_sort or are not sorted)\n\n";
  }

  // -------------------------------------------------------------------------
  // Results Display
  // -------------------------------------------------------------------------
  double std_ms = dur_std / 1000.0;
  double dt_ms = dur_dt / 1000.0;
  double std_avg_us = (double)dur_std / NUM_RUNS;
  double dt_avg_us = (double)dur_dt / NUM_RUNS;
  double speedup = (double)dur_std / dur_dt;

  std::cout << "----------------- RESULTS -----------------\n";
  std::cout << std::fixed << std::setprecision(3);
  std::cout << "std::stable_sort:\n";
  std::cout << "  Total time:   " << std::setw(8) << std_ms << " ms (for " << NUM_RUNS << " runs)\n";
  std::cout << "  Average/sort: " << std::setw(8) << std_avg_us << " us\n\n";

  std::cout << "dt_stable_sort:\n";
  std::cout << "  Total time:   " << std::setw(8) << dt_ms << " ms (for " << NUM_RUNS << " runs)\n";
  std::cout << "  Average/sort: " << std::setw(8) << dt_avg_us << " us\n\n";

  std::cout << "Speedup factor: " << speedup << "x\n";
  if (speedup > 1.0) {
    std::cout << "-> dt_stable_sort is " << (speedup - 1.0) * 100.0 << "% faster than std::stable_sort!\n";
  } else {
    std::cout << "-> std::stable_sort is " << (1.0 / speedup - 1.0) * 100.0 << "% faster than dt_stable_sort.\n";
  }
  std::cout << "-------------------------------------------\n";

  return passed ? 0 : 1;
}
