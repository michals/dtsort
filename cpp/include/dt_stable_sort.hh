#pragma once

// This header is designed for inclusion in a single translation unit.
// Including it from multiple TUs will result in duplicated internal code.

#include <algorithm>  // std::merge, std::inplace_merge, std::copy, std::move
#include <cassert>    // for generated header asserts
#include <cstddef>    // std::ptrdiff_t
#include <functional> // std::less
#include <iterator>   // std::iterator_traits
#include <new>        // ::operator new/delete with std::nothrow
#include <utility>    // std::swap
#include <vector>     // std::vector for buffer

#include <numeric> // std::iota
#include <type_traits> // std::is_trivially_copyable_v

#ifndef MAX_DT_SIZE
#define MAX_DT_SIZE 10
#endif

#ifndef DT_ADAPTIVE
// dtsort can be built with it disabled (0) for even higher performance for high entropy data,
// or with enabled adaptivity (1) that handles well already sorted or partially sorted data.
#define DT_ADAPTIVE 1
#endif

// Compiler hints for branch prediction
#if defined(__GNUC__) || defined(__clang__)
#define DT_LIKELY(x) (__builtin_expect(!!(x), 1))
#define DT_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
#define DT_LIKELY(x) (x)
#define DT_UNLIKELY(x) (x)
#endif

// Cache-Aware Index Block Size
#ifndef DT_BLOCK_SIZE
#define DT_BLOCK_SIZE (16 * MAX_DT_SIZE)
#endif

// ============================================================================
// GS10 Full Mode: Optimal DTs for N=2..5 + Group-Sorted DTs for N=6..10
// ============================================================================
// Uses the best available dtsort kernel for every size up to 10.
// This eliminates the insertion sort fallback for remainder leaves.
//
// IMPORTANT: If you change the optimal GS configuration for any N, you MUST update:
// 1. include/dt_stable_sort.hh (C++ includes and dispatch logic)
// 2. CMakeLists.txt (root: generate target)
// 3. cpp/benchmark/cpp/CMakeLists.txt (DT_MODE_GS file mappings)
// 4. cpp/benchmark/runner/db.py (get_dt_sizes file mapping logic)

#include "dtsort2_full.hh"
#include "dtsort3_full.hh"
#include "dtsort4_full.hh"
#include "dtsort5_full.hh"
#include "dtsort6_gs42.hh"
#include "dtsort7_gs52.hh"
#include "dtsort8_gs53.hh"
#include "dtsort9_gs54.hh"
#include "dtsort10_gs55.hh"


// Forward declaration of the public API
template <typename RandomAccessIterator, typename Compare>
void dt_stable_sort(RandomAccessIterator first, RandomAccessIterator last,
                    Compare comp);

namespace {

/**
 * @brief Dispatches to the optimal sorter based on range length.
 */
template <typename RandomAccessIterator, typename Compare>
inline void dt_leaf_sort_impl(RandomAccessIterator first,
                              RandomAccessIterator last, Compare comp) {
  auto len = last - first;

  if (DT_LIKELY(len == 10)) {
    dtsort10gs55(first, last, comp);
    return;
  }

  switch (len) {
  case 2: dtsort2(first, last, comp); break;
  case 3: dtsort3(first, last, comp); break;
  case 4: dtsort4(first, last, comp); break;
  case 5: dtsort5(first, last, comp); break;
  case 6: dtsort6gs42(first, last, comp); break;
  case 7: dtsort7gs52(first, last, comp); break;
  case 8: dtsort8gs53(first, last, comp); break;
  case 9: dtsort9gs54(first, last, comp); break;
  default: break;
  }
}

/**
 * @brief Helper to calculate block-aligned split point.
 */
inline std::ptrdiff_t dt_aligned_mid_impl(std::ptrdiff_t len) {
  std::ptrdiff_t mid = len / 2;
  mid = (mid / MAX_DT_SIZE) * MAX_DT_SIZE;
  if (mid == 0) {
    mid = MAX_DT_SIZE;
  }
  return mid;
}

/**
 * @brief Index-based sorting dispatch (Full GS10 version).
 */
template <typename RandomAccessIterator, typename OutputIterator,
          typename Compare>
inline void dt_leaf_sort_move_impl(RandomAccessIterator first,
                                   RandomAccessIterator last, OutputIterator dest,
                                   Compare comp) {
  using diff_t =
      typename std::iterator_traits<RandomAccessIterator>::difference_type;
  using value_t =
      typename std::iterator_traits<RandomAccessIterator>::value_type;

  diff_t len = last - first;

  if constexpr (sizeof(value_t) <= sizeof(void *) &&
                std::is_trivially_copyable_v<value_t>) {
    dt_leaf_sort_impl(first, last, comp);
    for (diff_t i = 0; i < len; ++i) {
      *dest++ = *(first + i);
    }
    return;
  }

  diff_t idx[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto idx_comp = [&](diff_t a, diff_t b) {
    return comp(*(first + a), *(first + b));
  };

  if (DT_LIKELY(len == 10)) {
    dtsort10gs55(&idx[0], &idx[0] + 10, idx_comp);
  } else {
    switch (len) {
    case 2: dtsort2(&idx[0], &idx[0] + 2, idx_comp); break;
    case 3: dtsort3(&idx[0], &idx[0] + 3, idx_comp); break;
    case 4: dtsort4(&idx[0], &idx[0] + 4, idx_comp); break;
    case 5: dtsort5(&idx[0], &idx[0] + 5, idx_comp); break;
    case 6: dtsort6gs42(&idx[0], &idx[0] + 6, idx_comp); break;
    case 7: dtsort7gs52(&idx[0], &idx[0] + 7, idx_comp); break;
    case 8: dtsort8gs53(&idx[0], &idx[0] + 8, idx_comp); break;
    case 9: dtsort9gs54(&idx[0], &idx[0] + 9, idx_comp); break;
    default: break;
    }
  }

  for (diff_t i = 0; i < len; ++i) {
    *dest++ = std::move(*(first + idx[i]));
  }
}

// Forward declarations for recursion
template <bool BlockSortEnabled = true, typename RandomAccessIterator,
          typename T, typename Compare>
void dt_stable_sort_impl(RandomAccessIterator first, RandomAccessIterator last,
                         T *buf, std::ptrdiff_t buf_size, Compare comp);

template <bool BlockSortEnabled = true, typename RandomAccessIterator,
          typename T, typename Compare>
inline void dt_stable_sort_move_impl(RandomAccessIterator first,
                                     RandomAccessIterator last, T *dest,
                                     Compare comp);

/**
 * @brief Cache-Aware Index Block Sorter.
 */
template <typename RandomAccessIterator, typename OutputIterator,
          typename Compare>
inline void dt_block_sort_move_impl(RandomAccessIterator first,
                                    RandomAccessIterator last, OutputIterator dest,
                                    Compare comp) {
  using diff_t =
      typename std::iterator_traits<RandomAccessIterator>::difference_type;
  diff_t len = last - first;

  diff_t idx[DT_BLOCK_SIZE];
  std::iota(&idx[0], &idx[len], 0);

  diff_t idx_buf[DT_BLOCK_SIZE];
  dt_stable_sort_impl<false>(
      &idx[0], &idx[len], &idx_buf[0], len,
      [&](diff_t a, diff_t b) { return comp(*(first + a), *(first + b)); });

  for (diff_t i = 0; i < len; ++i) {
    *(dest + i) = std::move(*(first + idx[i]));
  }
}

/**
 * @brief Ping-pong merge sort (move path).
 */
template <bool BlockSortEnabled, typename RandomAccessIterator, typename T,
          typename Compare>
inline void dt_stable_sort_move_impl(RandomAccessIterator first,
                                     RandomAccessIterator last, T *dest,
                                     Compare comp) {
  std::ptrdiff_t len = last - first;
  if (len <= MAX_DT_SIZE) {
    dt_leaf_sort_move_impl(first, last, dest, comp);
    return;
  }

  if constexpr (BlockSortEnabled) {
    if (len <= DT_BLOCK_SIZE) {
      dt_block_sort_move_impl(first, last, dest, comp);
      return;
    }
  }

  std::ptrdiff_t mid_len = dt_aligned_mid_impl(len);
  auto mid = first + mid_len;

  dt_stable_sort_impl<BlockSortEnabled>(first, mid, dest, mid_len, comp);
  dt_stable_sort_impl<BlockSortEnabled>(mid, last, dest + mid_len, len - mid_len,
                                        comp);

#if DT_ADAPTIVE
  if (!comp(*mid, *(mid - 1))) {
    std::move(first, mid, dest);
    std::move(mid, last, dest + mid_len);
    return;
  }
#endif

  std::merge(std::make_move_iterator(first), std::make_move_iterator(mid),
             std::make_move_iterator(mid), std::make_move_iterator(last), dest,
             comp);
}

/**
 * @brief Ping-pong merge sort (in-place path).
 */
template <bool BlockSortEnabled, typename RandomAccessIterator, typename T,
          typename Compare>
void dt_stable_sort_impl(RandomAccessIterator first, RandomAccessIterator last,
                         T *buf, std::ptrdiff_t buf_size, Compare comp) {
  std::ptrdiff_t len = last - first;
  if (len <= MAX_DT_SIZE) {
    dt_leaf_sort_impl(first, last, comp);
    return;
  }

  if constexpr (BlockSortEnabled) {
    if (len <= buf_size && len <= DT_BLOCK_SIZE) {
      dt_block_sort_move_impl(first, last, buf, comp);
      std::move(buf, buf + len, first);
      return;
    }
  }

  std::ptrdiff_t mid_len = dt_aligned_mid_impl(len);
  auto mid = first + mid_len;

  if (len <= buf_size) {
    dt_stable_sort_move_impl<BlockSortEnabled>(first, mid, buf, comp);
    dt_stable_sort_move_impl<BlockSortEnabled>(mid, last, buf + mid_len, comp);

#if DT_ADAPTIVE
    if (!comp(*(buf + mid_len), *(buf + mid_len - 1))) {
      std::move(buf, buf + len, first);
      return;
    }
#endif

    std::merge(std::make_move_iterator(buf),
               std::make_move_iterator(buf + mid_len),
               std::make_move_iterator(buf + mid_len),
               std::make_move_iterator(buf + len), first, comp);
    return;
  }

  dt_stable_sort_impl<BlockSortEnabled>(first, mid, buf, buf_size, comp);
  dt_stable_sort_impl<BlockSortEnabled>(mid, last, buf, buf_size, comp);
#if DT_ADAPTIVE
  if (!comp(*mid, *(mid - 1))) return;
#endif
  std::inplace_merge(first, mid, last, comp);
}

/**
 * @brief In-place stable sort fallback.
 */
template <typename RandomAccessIterator, typename Compare>
inline void dt_inplace_stable_sort_impl(RandomAccessIterator first,
                                        RandomAccessIterator last, Compare comp) {
  auto len = last - first;
  if (len <= MAX_DT_SIZE) {
    dt_leaf_sort_impl(first, last, comp);
    return;
  }
  std::ptrdiff_t mid_len = dt_aligned_mid_impl(len);
  auto mid = first + mid_len;
  dt_inplace_stable_sort_impl(first, mid, comp);
  dt_inplace_stable_sort_impl(mid, last, comp);
#if DT_ADAPTIVE
  if (!comp(*mid, *(mid - 1))) return;
#endif
  std::inplace_merge(first, mid, last, comp);
}

} // anonymous namespace

/**
 * @brief Standalone stable sort (GS10 Full).
 *
 * Uses optimal kernels for N=2..5 and group-sorted kernels for N=6..10.
 */
template <typename RandomAccessIterator, typename Compare>
void dt_stable_sort(RandomAccessIterator first, RandomAccessIterator last,
                    Compare comp) {
  using T = typename std::iterator_traits<RandomAccessIterator>::value_type;
  std::ptrdiff_t len = last - first;

  if (len <= 1)
    return;

  if (len <= MAX_DT_SIZE) {
    dt_leaf_sort_impl(first, last, comp);
    return;
  }

  try {
    std::vector<T> buf(len);
    constexpr bool EnableBlockSort = sizeof(T) > 32;
    dt_stable_sort_impl<EnableBlockSort>(first, last, buf.data(), len, comp);
  } catch (const std::bad_alloc &) {
    dt_inplace_stable_sort_impl(first, last, comp);
  }
}


/**
 * @brief Overload for default comparator.
 */
template <typename RandomAccessIterator>
inline void dt_stable_sort(RandomAccessIterator first,
                           RandomAccessIterator last) {
  dt_stable_sort(
      first, last,
      std::less<
          typename std::iterator_traits<RandomAccessIterator>::value_type>{});
}

namespace dtsort {
using ::dt_stable_sort;
}
