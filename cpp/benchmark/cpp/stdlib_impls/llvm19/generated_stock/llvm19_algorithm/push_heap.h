#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_PUSH_HEAP_H
#define _LLVM19___ALGORITHM_PUSH_HEAP_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/comp_ref_type.h"
#include "llvm19_algorithm/iterator_operations.h"
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
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 void
__sift_up(_RandomAccessIterator __first,
          _RandomAccessIterator __last,
          _Compare&& __comp,
          typename iterator_traits<_RandomAccessIterator>::difference_type __len) {
  using value_type = typename iterator_traits<_RandomAccessIterator>::value_type;

  if (__len > 1) {
    __len                       = (__len - 2) / 2;
    _RandomAccessIterator __ptr = __first + __len;

    if (__comp(*__ptr, *--__last)) {
      value_type __t(_IterOps<_AlgPolicy>::__iter_move(__last));
      do {
        *__last = _IterOps<_AlgPolicy>::__iter_move(__ptr);
        __last  = __ptr;
        if (__len == 0)
          break;
        __len = (__len - 1) / 2;
        __ptr = __first + __len;
      } while (__comp(*__ptr, __t));

      *__last = target_std::move(__t);
    }
  }
}

template <class _AlgPolicy, class _RandomAccessIterator, class _Compare>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 void
__push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare& __comp) {
  typename iterator_traits<_RandomAccessIterator>::difference_type __len = __last - __first;
  target_std::__sift_up<_AlgPolicy, __comp_ref_type<_Compare> >(target_std::move(__first), target_std::move(__last), __comp, __len);
}

template <class _RandomAccessIterator, class _Compare>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp) {
  static_assert(target_std::is_copy_constructible<_RandomAccessIterator>::value, "Iterators must be copy constructible.");
  static_assert(target_std::is_copy_assignable<_RandomAccessIterator>::value, "Iterators must be copy assignable.");

  target_std::__push_heap<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__last), __comp);
}

template <class _RandomAccessIterator>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 void
push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last) {
  target_std::push_heap(target_std::move(__first), target_std::move(__last), __less<>());
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_PUSH_HEAP_H
