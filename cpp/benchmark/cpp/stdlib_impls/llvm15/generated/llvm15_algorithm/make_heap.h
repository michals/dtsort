#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_MAKE_HEAP_H
#define _LLVM15___ALGORITHM_MAKE_HEAP_H

#include "llvm15_algorithm/comp.h"
#include "llvm15_algorithm/comp_ref_type.h"
#include "llvm15_algorithm/iterator_operations.h"
#include "llvm15_algorithm/sift_down.h"
#include "llvm15_config"
#include "llvm15_iterator/iterator_traits.h"
#include "llvm15_utility/move.h"

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

template <class _AlgPolicy, class _Compare, class _RandomAccessIterator>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
void __make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare&& __comp) {
  using _CompRef = typename __comp_ref_type<_Compare>::type;
  _CompRef __comp_ref = __comp;

  using difference_type = typename iterator_traits<_RandomAccessIterator>::difference_type;
  difference_type __n = __last - __first;
  if (__n > 1) {
    // start from the first parent, there is no need to consider children
    for (difference_type __start = (__n - 2) / 2; __start >= 0; --__start) {
        target_std::__sift_down<_AlgPolicy>(__first, __comp_ref, __n, __first + __start);
    }
  }
}

template <class _RandomAccessIterator, class _Compare>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
void make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp) {
  target_std::__make_heap<_ClassicAlgPolicy>(target_std::move(__first), target_std::move(__last), __comp);
}

template <class _RandomAccessIterator>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
void make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last) {
  target_std::make_heap(target_std::move(__first), target_std::move(__last),
      __less<typename iterator_traits<_RandomAccessIterator>::value_type>());
}

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___ALGORITHM_MAKE_HEAP_H
