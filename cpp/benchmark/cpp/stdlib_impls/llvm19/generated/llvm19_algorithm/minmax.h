#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_MINMAX_H
#define _LLVM19___ALGORITHM_MINMAX_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/minmax_element.h"
#include "llvm19_config"
#include "llvm19_functional/identity.h"
#include "llvm19_type_traits/is_callable.h"
#include "llvm19_utility/pair.h"
#include <initializer_list>

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_BEGIN_NAMESPACE_STD

template <class _Tp, class _Compare>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<const _Tp&, const _Tp&>
minmax(_LLVM19_LIFETIMEBOUND const _Tp& __a, _LLVM19_LIFETIMEBOUND const _Tp& __b, _Compare __comp) {
  return __comp(__b, __a) ? pair<const _Tp&, const _Tp&>(__b, __a) : pair<const _Tp&, const _Tp&>(__a, __b);
}

template <class _Tp>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<const _Tp&, const _Tp&>
minmax(_LLVM19_LIFETIMEBOUND const _Tp& __a, _LLVM19_LIFETIMEBOUND const _Tp& __b) {
  return target_std::minmax(__a, __b, __less<>());
}

#ifndef _LLVM19_CXX03_LANG

template <class _Tp, class _Compare>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_Tp, _Tp>
minmax(initializer_list<_Tp> __t, _Compare __comp) {
  static_assert(__is_callable<_Compare, _Tp, _Tp>::value, "The comparator has to be callable");
  __identity __proj;
  auto __ret = target_std::__minmax_element_impl(__t.begin(), __t.end(), __comp, __proj);
  return pair<_Tp, _Tp>(*__ret.first, *__ret.second);
}

template <class _Tp>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_Tp, _Tp>
minmax(initializer_list<_Tp> __t) {
  return target_std::minmax(__t, __less<>());
}

#endif // _LLVM19_CXX03_LANG

_LLVM19_END_NAMESPACE_STD

#endif // _LLVM19___ALGORITHM_MINMAX_H
