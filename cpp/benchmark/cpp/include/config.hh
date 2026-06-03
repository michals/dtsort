#pragma once

#include <array>
#include <cstddef>

namespace bench_config {

// Maximum dtsort chunk size (set via CMake -DDTSORT_MAX_N=...)
#ifndef MAX_DT_SIZE
#define MAX_DT_SIZE 6
#endif

// Dataset sizes to benchmark
constexpr std::array<size_t, 4> DATA_SIZES = {1000, 10000, 100000, 1000000};

// Number of trials per (impl, type, size) cell
constexpr int DEFAULT_TRIALS = 30;

// Smoke test uses smaller values
constexpr size_t SMOKE_TEST_SIZE = 200;
constexpr int SMOKE_TEST_TRIALS = 1;

} // namespace bench_config
