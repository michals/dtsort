#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_POP_HEAP_H
#define _LLVM19___ALGORITHM_POP_HEAP_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/comp_ref_type.h"
#include "llvm19_algorithm/iterator_operations.h"
#include "llvm19_algorithm/push_heap.h"
#include "llvm19_algorithm/sift_down.h"
#include <__assert>
#include "llvm19_config"
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
__pop_heap(_RandomAccessIterator __first,
           _RandomAccessIterator __last,
           _Compare& __comp,
           typename iterator_traits<_RandomAccessIterator>::difference_type __len) {
  // Calling `pop_heap` on an empty range is undefined behavior, but in practice it will be a no-op.
  _LLVM19_ASSERT_PEDANTIC(__len > 0, "The heap given to pop_heap must be non-empty");

  __comp_ref_type<_Compare> __comp_ref = __comp;

  using value_type = typename iterator_traits<_RandomAccessIterator>::value_type;
  if (__len > 1) {
    value_type __top             = _IterOps<_AlgPolicy>::__iter_move(__first); // create a hole at __first
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
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp) {
  static_assert(target_std::is_copy_constructible<_RandomAccessIterator>::value, "Iterators must be copy constructible.");
  static_assert(target_std::is_copy_assignable<_RandomAccessIterator>::value, "Iterators must be copy assignable.");

  typename iterator_traits<_RandomAccessIterator>::difference_type __len = __last - __first;
  target_std::__pop_heap<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__last), __comp, __len);
}

template <class _RandomAccessIterator>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last) {
  target_std::pop_heap(target_std::move(__first), target_std::move(__last), __less<>());
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_POP_HEAP_H
