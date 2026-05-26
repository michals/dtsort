#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_SORT_HEAP_H
#define _LLVM19___ALGORITHM_SORT_HEAP_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/comp_ref_type.h"
#include "llvm19_algorithm/iterator_operations.h"
#include "llvm19_algorithm/pop_heap.h"
#include "llvm19_config"
#include "llvm19_debug_utils/strict_weak_ordering_check.h"
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

template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 void
__sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare&& __comp) {
  _RandomAccessIterator __saved_last   = __last;
  __comp_ref_type<_Compare> __comp_ref = __comp;

  using difference_type = typename iterator_traits<_RandomAccessIterator>::difference_type;
  for (difference_type __n = __last - __first; __n > 1; --__last, (void)--__n)
    target_std::__pop_heap<_AlgPolicy>(__first, __last, __comp_ref, __n);
  target_std::__check_strict_weak_ordering_sorted(__first, __saved_last, __comp_ref);
}

template <class _RandomAccessIterator, class _Compare>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp) {
  static_assert(target_std::is_copy_constructible<_RandomAccessIterator>::value, "Iterators must be copy constructible.");
  static_assert(target_std::is_copy_assignable<_RandomAccessIterator>::value, "Iterators must be copy assignable.");

  target_std::__sort_heap<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__last), __comp);
}

template <class _RandomAccessIterator>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last) {
  target_std::sort_heap(target_std::move(__first), target_std::move(__last), __less<>());
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_SORT_HEAP_H
