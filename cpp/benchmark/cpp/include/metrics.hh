#pragma once

#include <cstdint>

// Thread-local instrumentation counters for benchmark measurements.
// Usage:
//   metrics::reset();
//   /* ... sort ... */
//   auto m = metrics::get();
//   // m.comparisons, m.moves, m.copies

namespace metrics {

struct Metrics {
  uint64_t comparisons = 0;
  uint64_t moves = 0;
  uint64_t copies = 0;
  uint64_t dtsort_comparisons = 0;
  uint64_t dtsort_moves = 0;
  uint64_t dtsort_copies = 0;
};

namespace detail {
inline thread_local Metrics counters;
inline thread_local bool in_dtsort = false;
} // namespace detail

struct ScopedDtsortMetrics {
  bool prev;
  ScopedDtsortMetrics() {
    prev = detail::in_dtsort;
    detail::in_dtsort = true;
  }
  ~ScopedDtsortMetrics() { detail::in_dtsort = prev; }
  ScopedDtsortMetrics(const ScopedDtsortMetrics &) = delete;
  ScopedDtsortMetrics &operator=(const ScopedDtsortMetrics &) = delete;
  ScopedDtsortMetrics(ScopedDtsortMetrics &&) = delete;
  ScopedDtsortMetrics &operator=(ScopedDtsortMetrics &&) = delete;
};

inline void reset() {
  detail::counters = Metrics{};
  detail::in_dtsort = false;
}

inline Metrics get() { return detail::counters; }

inline void count_comparison() {
  ++detail::counters.comparisons;
  if (detail::in_dtsort)
    ++detail::counters.dtsort_comparisons;
}
inline void count_move() {
  ++detail::counters.moves;
  if (detail::in_dtsort)
    ++detail::counters.dtsort_moves;
}
inline void count_copy() {
  ++detail::counters.copies;
  if (detail::in_dtsort)
    ++detail::counters.dtsort_copies;
}

} // namespace metrics
