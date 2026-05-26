#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_UPPER_BOUND_H
#define _LLVM19___ALGORITHM_UPPER_BOUND_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/half_positive.h"
#include "llvm19_algorithm/iterator_operations.h"
#include "llvm19_config"
#include "llvm19_functional/identity.h"
#include "llvm19_functional/invoke.h"
#include "llvm19_iterator/advance.h"
#include "llvm19_iterator/distance.h"
#include "llvm19_iterator/iterator_traits.h"
#include "llvm19_type_traits/is_constructible.h"
#include "llvm19_utility/move.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

template <class _AlgPolicy, class _Compare, class _Iter, class _Sent, class _Tp, class _Proj>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 _Iter
__upper_bound(_Iter __first, _Sent __last, const _Tp& __value, _Compare&& __comp, _Proj&& __proj) {
  auto __len = _IterOps<_AlgPolicy>::distance(__first, __last);
  while (__len != 0) {
    auto __half_len = target_std::__half_positive(__len);
    auto __mid      = _IterOps<_AlgPolicy>::next(__first, __half_len);
    if (target_std::__invoke(__comp, __value, target_std::__invoke(__proj, *__mid)))
      __len = __half_len;
    else {
      __first = ++__mid;
      __len -= __half_len + 1;
    }
  }
  return __first;
}

template <class _ForwardIterator, class _Tp, class _Compare>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 _ForwardIterator
upper_bound(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value, _Compare __comp) {
  static_assert(is_copy_constructible<_ForwardIterator>::value, "Iterator has to be copy constructible");
  return target_std::__upper_bound<_ClassicAlgPolicy>(
      target_std::move(__first), target_std::move(__last), __value, target_std::move(__comp), target_std::__identity());
}

template <class _ForwardIterator, class _Tp>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 _ForwardIterator
upper_bound(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value) {
  return target_std::upper_bound(target_std::move(__first), target_std::move(__last), __value, __less<>());
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_UPPER_BOUND_H
