#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_PARTIAL_SORT_H
#define _LLVM19___ALGORITHM_PARTIAL_SORT_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/comp_ref_type.h"
#include "llvm19_algorithm/iterator_operations.h"
#include "llvm19_algorithm/make_heap.h"
#include "llvm19_algorithm/sift_down.h"
#include "llvm19_algorithm/sort_heap.h"
#include "llvm19_config"
#include "llvm19_debug_utils/randomize_range.h"
#include "llvm19_iterator/iterator_traits.h"
#include "llvm19_type_traits/is_assignable.h"
#include "llvm19_type_traits/is_constructible.h"
#include "llvm19_utility/move.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

template <class _AlgPolicy, class _Compare, class _RandomAccessIterator, class _Sentinel>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 _RandomAccessIterator __partial_sort_impl(
    _RandomAccessIterator __first, _RandomAccessIterator __middle, _Sentinel __last, _Compare&& __comp) {
  if (__first == __middle) {
    return _IterOps<_AlgPolicy>::next(__middle, __last);
  }

  target_std::__make_heap<_AlgPolicy>(__first, __middle, __comp);

  typename iterator_traits<_RandomAccessIterator>::difference_type __len = __middle - __first;
  _RandomAccessIterator __i                                              = __middle;
  for (; __i != __last; ++__i) {
    if (__comp(*__i, *__first)) {
      _IterOps<_AlgPolicy>::iter_swap(__i, __first);
      target_std::__sift_down<_AlgPolicy>(__first, __comp, __len, __first);
    }
  }
  target_std::__sort_heap<_AlgPolicy>(target_std::move(__first), target_std::move(__middle), __comp);

  return __i;
}

template <class _AlgPolicy, class _Compare, class _RandomAccessIterator, class _Sentinel>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 _RandomAccessIterator
__partial_sort(_RandomAccessIterator __first, _RandomAccessIterator __middle, _Sentinel __last, _Compare& __comp) {
  if (__first == __middle)
    return _IterOps<_AlgPolicy>::next(__middle, __last);

  target_std::__debug_randomize_range<_AlgPolicy>(__first, __last);

  auto __last_iter =
      target_std::__partial_sort_impl<_AlgPolicy>(__first, __middle, __last, static_cast<__comp_ref_type<_Compare> >(__comp));

  target_std::__debug_randomize_range<_AlgPolicy>(__middle, __last);

  return __last_iter;
}

template <class _RandomAccessIterator, class _Compare>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void partial_sort(
    _RandomAccessIterator __first, _RandomAccessIterator __middle, _RandomAccessIterator __last, _Compare __comp) {
  static_assert(target_std::is_copy_constructible<_RandomAccessIterator>::value, "Iterators must be copy constructible.");
  static_assert(target_std::is_copy_assignable<_RandomAccessIterator>::value, "Iterators must be copy assignable.");

  (void)target_std::__partial_sort<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__middle), target_std::move(__last), __comp);
}

template <class _RandomAccessIterator>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
partial_sort(_RandomAccessIterator __first, _RandomAccessIterator __middle, _RandomAccessIterator __last) {
  target_std::partial_sort(__first, __middle, __last, __less<>());
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_PARTIAL_SORT_H
