#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_LOWER_BOUND_H
#define _LLVM15___ALGORITHM_LOWER_BOUND_H

#include "llvm15_algorithm/comp.h"
#include "llvm15_algorithm/half_positive.h"
#include "llvm15_algorithm/iterator_operations.h"
#include "llvm15_config"
#include "llvm15_functional/identity.h"
#include "llvm15_functional/invoke.h"
#include "llvm15_iterator/advance.h"
#include "llvm15_iterator/distance.h"
#include "llvm15_iterator/iterator_traits.h"
#include "llvm15_type_traits/is_callable.h"
#include "llvm15_type_traits/remove_reference.h"
#include <type_traits>

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

template <class _AlgPolicy, class _Iter, class _Sent, class _Type, class _Proj, class _Comp>
_LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
_Iter __lower_bound_impl(_Iter __first, _Sent __last, const _Type& __value, _Comp& __comp, _Proj& __proj) {
  auto __len = _IterOps<_AlgPolicy>::distance(__first, __last);

  while (__len != 0) {
    auto __l2 = target_std::__half_positive(__len);
    _Iter __m = __first;
    _IterOps<_AlgPolicy>::advance(__m, __l2);
    if (target_std::__invoke(__comp, target_std::__invoke(__proj, *__m), __value)) {
      __first = ++__m;
      __len -= __l2 + 1;
    } else {
      __len = __l2;
    }
  }
  return __first;
}

template <class _ForwardIterator, class _Tp, class _Compare>
_LLVM15_NODISCARD_EXT inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
_ForwardIterator lower_bound(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value, _Compare __comp) {
  static_assert(__is_callable<_Compare, decltype(*__first), const _Tp&>::value,
                "The comparator has to be callable");
  auto __proj = target_std::__identity();
  return target_std::__lower_bound_impl<_ClassicAlgPolicy>(__first, __last, __value, __comp, __proj);
}

template <class _ForwardIterator, class _Tp>
_LLVM15_NODISCARD_EXT inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
_ForwardIterator lower_bound(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value) {
  return target_std::lower_bound(__first, __last, __value,
                          __less<typename iterator_traits<_ForwardIterator>::value_type, _Tp>());
}

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___ALGORITHM_LOWER_BOUND_H
