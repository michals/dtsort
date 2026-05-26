#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_MIN_ELEMENT_H
#define _LLVM19___ALGORITHM_MIN_ELEMENT_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/comp_ref_type.h"
#include "llvm19_config"
#include "llvm19_functional/identity.h"
#include "llvm19_functional/invoke.h"
#include "llvm19_iterator/iterator_traits.h"
#include "llvm19_type_traits/is_callable.h"
#include "llvm19_utility/move.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

template <class _Comp, class _Iter, class _Sent, class _Proj>
inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 _Iter
__min_element(_Iter __first, _Sent __last, _Comp __comp, _Proj& __proj) {
  if (__first == __last)
    return __first;

  _Iter __i = __first;
  while (++__i != __last)
    if (target_std::__invoke(__comp, target_std::__invoke(__proj, *__i), target_std::__invoke(__proj, *__first)))
      __first = __i;

  return __first;
}

template <class _Comp, class _Iter, class _Sent>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 _Iter __min_element(_Iter __first, _Sent __last, _Comp __comp) {
  auto __proj = __identity();
  return target_std::__min_element<_Comp>(target_std::move(__first), target_std::move(__last), __comp, __proj);
}

template <class _ForwardIterator, class _Compare>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 _ForwardIterator
min_element(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp) {
  static_assert(
      __has_forward_iterator_category<_ForwardIterator>::value, "target_std::min_element requires a ForwardIterator");
  static_assert(
      __is_callable<_Compare, decltype(*__first), decltype(*__first)>::value, "The comparator has to be callable");

  return target_std::__min_element<__comp_ref_type<_Compare> >(target_std::move(__first), target_std::move(__last), __comp);
}

template <class _ForwardIterator>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 _ForwardIterator
min_element(_ForwardIterator __first, _ForwardIterator __last) {
  return target_std::min_element(__first, __last, __less<>());
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_MIN_ELEMENT_H
