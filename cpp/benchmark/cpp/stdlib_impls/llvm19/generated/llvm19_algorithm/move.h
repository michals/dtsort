#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_MOVE_H
#define _LLVM19___ALGORITHM_MOVE_H

#include "llvm19_algorithm/copy_move_common.h"
#include "llvm19_algorithm/for_each_segment.h"
#include "llvm19_algorithm/iterator_operations.h"
#include "llvm19_algorithm/min.h"
#include "llvm19_config"
#include "llvm19_iterator/segmented_iterator.h"
#include "llvm19_type_traits/common_type.h"
#include "llvm19_type_traits/is_constructible.h"
#include "llvm19_utility/move.h"
#include "llvm19_utility/pair.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

template <class _AlgPolicy, class _InIter, class _Sent, class _OutIter>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_InIter, _OutIter>
__move(_InIter __first, _Sent __last, _OutIter __result);

template <class _AlgPolicy>
struct __move_impl {
  template <class _InIter, class _Sent, class _OutIter>
  _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_InIter, _OutIter>
  operator()(_InIter __first, _Sent __last, _OutIter __result) const {
    while (__first != __last) {
      *__result = _IterOps<_AlgPolicy>::__iter_move(__first);
      ++__first;
      ++__result;
    }
    return target_std::make_pair(target_std::move(__first), target_std::move(__result));
  }

  template <class _InIter, class _OutIter>
  struct _MoveSegment {
    using _Traits = __segmented_iterator_traits<_InIter>;

    _OutIter& __result_;

    _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 explicit _MoveSegment(_OutIter& __result)
        : __result_(__result) {}

    _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 void
    operator()(typename _Traits::__local_iterator __lfirst, typename _Traits::__local_iterator __llast) {
      __result_ = target_std::__move<_AlgPolicy>(__lfirst, __llast, target_std::move(__result_)).second;
    }
  };

  template <class _InIter, class _OutIter, __enable_if_t<__is_segmented_iterator<_InIter>::value, int> = 0>
  _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_InIter, _OutIter>
  operator()(_InIter __first, _InIter __last, _OutIter __result) const {
    target_std::__for_each_segment(__first, __last, _MoveSegment<_InIter, _OutIter>(__result));
    return target_std::make_pair(__last, target_std::move(__result));
  }

  template <class _InIter,
            class _OutIter,
            __enable_if_t<__has_random_access_iterator_category<_InIter>::value &&
                              !__is_segmented_iterator<_InIter>::value && __is_segmented_iterator<_OutIter>::value,
                          int> = 0>
  _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_InIter, _OutIter>
  operator()(_InIter __first, _InIter __last, _OutIter __result) const {
    using _Traits = __segmented_iterator_traits<_OutIter>;
    using _DiffT  = typename common_type<__iter_diff_t<_InIter>, __iter_diff_t<_OutIter> >::type;

    if (__first == __last)
      return target_std::make_pair(target_std::move(__first), target_std::move(__result));

    auto __local_first      = _Traits::__local(__result);
    auto __segment_iterator = _Traits::__segment(__result);
    while (true) {
      auto __local_last = _Traits::__end(__segment_iterator);
      auto __size       = target_std::min<_DiffT>(__local_last - __local_first, __last - __first);
      auto __iters      = target_std::__move<_AlgPolicy>(__first, __first + __size, __local_first);
      __first           = target_std::move(__iters.first);

      if (__first == __last)
        return target_std::make_pair(target_std::move(__first), _Traits::__compose(__segment_iterator, target_std::move(__iters.second)));

      __local_first = _Traits::__begin(++__segment_iterator);
    }
  }

  // At this point, the iterators have been unwrapped so any `contiguous_iterator` has been unwrapped to a pointer.
  template <class _In, class _Out, __enable_if_t<__can_lower_move_assignment_to_memmove<_In, _Out>::value, int> = 0>
  _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_In*, _Out*>
  operator()(_In* __first, _In* __last, _Out* __result) const {
    return target_std::__copy_trivial_impl(__first, __last, __result);
  }
};

template <class _AlgPolicy, class _InIter, class _Sent, class _OutIter>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_InIter, _OutIter>
__move(_InIter __first, _Sent __last, _OutIter __result) {
  return target_std::__copy_move_unwrap_iters<__move_impl<_AlgPolicy> >(
      target_std::move(__first), target_std::move(__last), target_std::move(__result));
}

template <class _InputIterator, class _OutputIterator>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 _OutputIterator
move(_InputIterator __first, _InputIterator __last, _OutputIterator __result) {
  static_assert(is_copy_constructible<_InputIterator>::value, "Iterators has to be copy constructible.");
  static_assert(is_copy_constructible<_OutputIterator>::value, "The output iterator has to be copy constructible.");

  return target_std::__move<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__last), target_std::move(__result)).second;
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_MOVE_H
