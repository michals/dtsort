#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_MIN_ELEMENT_H
#define _LLVM15___ALGORITHM_MIN_ELEMENT_H

#include "llvm15_algorithm/comp.h"
#include "llvm15_algorithm/comp_ref_type.h"
#include "llvm15_config"
#include "llvm15_functional/identity.h"
#include "llvm15_functional/invoke.h"
#include "llvm15_iterator/iterator_traits.h"
#include "llvm15_type_traits/is_callable.h"
#include "llvm15_utility/move.h"

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

template <class _Comp, class _Iter, class _Sent, class _Proj>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
_Iter __min_element(_Iter __first, _Sent __last, _Comp __comp, _Proj& __proj) {
  if (__first == __last)
    return __first;

  _Iter __i = __first;
  while (++__i != __last)
    if (target_std::__invoke(__comp, target_std::__invoke(__proj, *__i), target_std::__invoke(__proj, *__first)))
      __first = __i;

  return __first;
}

template <class _Comp, class _Iter, class _Sent>
_LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
_Iter __min_element(_Iter __first, _Sent __last, _Comp __comp) {
  auto __proj = __identity();
  return target_std::__min_element<_Comp>(target_std::move(__first), target_std::move(__last), __comp, __proj);
}

template <class _ForwardIterator, class _Compare>
_LLVM15_NODISCARD_EXT inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11 _ForwardIterator
min_element(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
  static_assert(__is_cpp17_forward_iterator<_ForwardIterator>::value,
      "target_std::min_element requires a ForwardIterator");
  static_assert(__is_callable<_Compare, decltype(*__first), decltype(*__first)>::value,
              "The comparator has to be callable");

  typedef typename __comp_ref_type<_Compare>::type _Comp_ref;
  return target_std::__min_element<_Comp_ref>(target_std::move(__first), target_std::move(__last), __comp);
}

template <class _ForwardIterator>
_LLVM15_NODISCARD_EXT inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11 _ForwardIterator
min_element(_ForwardIterator __first, _ForwardIterator __last)
{
    return _VSTD::min_element(__first, __last,
              __less<typename iterator_traits<_ForwardIterator>::value_type>());
}

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___ALGORITHM_MIN_ELEMENT_H
