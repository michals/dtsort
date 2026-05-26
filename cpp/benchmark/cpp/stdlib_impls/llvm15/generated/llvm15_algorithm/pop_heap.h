#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_POP_HEAP_H
#define _LLVM15___ALGORITHM_POP_HEAP_H

#include "llvm15_algorithm/comp.h"
#include "llvm15_algorithm/comp_ref_type.h"
#include "llvm15_algorithm/iterator_operations.h"
#include "llvm15_algorithm/push_heap.h"
#include "llvm15_algorithm/sift_down.h"
#include <__assert>
#include "llvm15_config"
#include "llvm15_iterator/iterator_traits.h"
#include "llvm15_utility/move.h"
#include <type_traits>

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
void __pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare& __comp,
    typename iterator_traits<_RandomAccessIterator>::difference_type __len) {
  _LLVM15_ASSERT(__len > 0, "The heap given to pop_heap must be non-empty");

  using _CompRef = typename __comp_ref_type<_Compare>::type;
  _CompRef __comp_ref = __comp;

  using value_type = typename iterator_traits<_RandomAccessIterator>::value_type;
  if (__len > 1) {
    value_type __top = _IterOps<_AlgPolicy>::__iter_move(__first);  // create a hole at __first
    _RandomAccessIterator __hole = target_std::__floyd_sift_down<_AlgPolicy>(__first, __comp_ref, __len);
    --__last;

    if (__hole == __last) {
      *__hole = target_std::move(__top);
    } else {
      *__hole = _IterOps<_AlgPolicy>::__iter_move(__last);
      ++__hole;
      *__last = target_std::move(__top);
      target_std::__sift_up<_AlgPolicy>(__first, __hole, __comp_ref, __hole - __first);
    }
  }
}

template <class _RandomAccessIterator, class _Compare>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
void pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp) {
  static_assert(target_std::is_copy_constructible<_RandomAccessIterator>::value, "Iterators must be copy constructible.");
  static_assert(target_std::is_copy_assignable<_RandomAccessIterator>::value, "Iterators must be copy assignable.");

  typename iterator_traits<_RandomAccessIterator>::difference_type __len = __last - __first;
  target_std::__pop_heap<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__last), __comp, __len);
}

template <class _RandomAccessIterator>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
void pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last) {
  target_std::pop_heap(target_std::move(__first), target_std::move(__last),
      __less<typename iterator_traits<_RandomAccessIterator>::value_type>());
}

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___ALGORITHM_POP_HEAP_H
