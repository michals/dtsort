#pragma once

#include "metrics.hh"
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

// MAX_DT_SIZE is defined via CMake (usually 6, 7, or 8)
// DT_THRESHOLD is defined via CMake (for ps-hybrid mode)

#if defined(DT_MODE_GS)
// ========================================================================
// GS Optimal mode: one hand-picked best group-sorted configuration per N.
// N=2..5: use full DTs (no pre-sorting needed — already optimal).
// N=6+: use group-sorted DTs with pre-sort kernels.
// ========================================================================
#include "dtsort2_full.hh"
#define DT_SUFFIX_2
#include "dtsort3_full.hh"
#define DT_SUFFIX_3
#include "dtsort4_full.hh"
#define DT_SUFFIX_4
#include "dtsort5_full.hh"
#define DT_SUFFIX_5
#if MAX_DT_SIZE >= 6
#include "dtsort6_gs42.hh"
#define DT_SUFFIX_6 gs42
#endif
#if MAX_DT_SIZE >= 7
#include "dtsort7_gs52.hh"
#define DT_SUFFIX_7 gs52
#endif
#if MAX_DT_SIZE >= 8
#include "dtsort8_gs53.hh"
#define DT_SUFFIX_8 gs53
#endif
#if MAX_DT_SIZE >= 9
#include "dtsort9_gs54.hh"
#define DT_SUFFIX_9 gs54
#endif
#if MAX_DT_SIZE >= 10
#include "dtsort10_gs55.hh"
#define DT_SUFFIX_10 gs55
#endif

#elif defined(MAX_DT_SIZE)
// ========================================================================
// Standard mode: full or ps/hybrid based on DT_THRESHOLD.
// ========================================================================
#if MAX_DT_SIZE >= 2
#if 2 >= DT_THRESHOLD
#include "dtsort2_ps.hh"
#define DT_SUFFIX_2 ps
#else
#include "dtsort2_full.hh"
#define DT_SUFFIX_2
#endif
#endif

#if MAX_DT_SIZE >= 3
#if 3 >= DT_THRESHOLD
#include "dtsort3_ps.hh"
#define DT_SUFFIX_3 ps
#else
#include "dtsort3_full.hh"
#define DT_SUFFIX_3
#endif
#endif
#if MAX_DT_SIZE >= 4
#if 4 >= DT_THRESHOLD
#include "dtsort4_ps.hh"
#define DT_SUFFIX_4 ps
#else
#include "dtsort4_full.hh"
#define DT_SUFFIX_4
#endif
#endif
#if MAX_DT_SIZE >= 5
#if 5 >= DT_THRESHOLD
#include "dtsort5_ps.hh"
#define DT_SUFFIX_5 ps
#else
#include "dtsort5_full.hh"
#define DT_SUFFIX_5
#endif
#endif
#if MAX_DT_SIZE >= 6
#if 6 >= DT_THRESHOLD
#include "dtsort6_ps.hh"
#define DT_SUFFIX_6 ps
#else
#include "dtsort6_full.hh"
#define DT_SUFFIX_6
#endif
#endif
#if MAX_DT_SIZE >= 7
#if 7 >= DT_THRESHOLD
#include "dtsort7_ps.hh"
#define DT_SUFFIX_7 ps
#else
#include "dtsort7_full.hh"
#define DT_SUFFIX_7
#endif
#endif
#if MAX_DT_SIZE >= 8
#if 8 >= DT_THRESHOLD
#include "dtsort8_ps.hh"
#define DT_SUFFIX_8 ps
#else
#include "dtsort8_full.hh"
#define DT_SUFFIX_8
#endif
#endif
#if MAX_DT_SIZE >= 9
#if 9 >= DT_THRESHOLD
#include "dtsort9_ps.hh"
#define DT_SUFFIX_9 ps
#else
#include "dtsort9_full.hh"
#define DT_SUFFIX_9
#endif
#endif
#if MAX_DT_SIZE >= 10
#if 10 >= DT_THRESHOLD
#include "dtsort10_ps.hh"
#define DT_SUFFIX_10 ps
#else
#include "dtsort10_full.hh"
#define DT_SUFFIX_10
#endif
#endif
#endif

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define DT_FUNC(n, suffix) CONCAT(dtsort, CONCAT(n, suffix))

/** @brief Central dispatch function for dtsort. Selects the optimal fixed-size
 * sorter based on the input range size. */
template <typename RandomAccessIterator, typename Compare>
inline void dt_dispatch(RandomAccessIterator first, RandomAccessIterator last,
                        Compare compare) {
  auto len = last - first;

  // We only use ScopedDtsortMetrics if we actually enter a dtsort branch.
  // This prevents insertion sort fallbacks from polluting dtsort-specific
  // metrics.

  switch (len) {
#ifdef DT_SUFFIX_2
  case 2: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(2, DT_SUFFIX_2)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_3
  case 3: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(3, DT_SUFFIX_3)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_4
  case 4: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(4, DT_SUFFIX_4)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_5
  case 5: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(5, DT_SUFFIX_5)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_6
  case 6: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(6, DT_SUFFIX_6)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_7
  case 7: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(7, DT_SUFFIX_7)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_8
  case 8: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(8, DT_SUFFIX_8)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_9
  case 9: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(9, DT_SUFFIX_9)(first, last, compare);
    return;
  }
#endif
#ifdef DT_SUFFIX_10
  case 10: {
    metrics::ScopedDtsortMetrics m;
    DT_FUNC(10, DT_SUFFIX_10)(first, last, compare);
    return;
  }
#endif
  default:
    // Fallback: Insertion sort for small ranges not covered by DT, or if len >
    // MAX_DT_SIZE. Note: This does NOT trigger ScopedDtsortMetrics.
    if (len > 1) {
      for (auto it = first + 1; it != last; ++it) {
        auto val = std::move(*it);
        auto cur = it;
        while (cur != first && compare(val, *(cur - 1))) {
          *cur = std::move(*(cur - 1));
          --cur;
        }
        *cur = std::move(val);
      }
    }
    return;
  }
}
