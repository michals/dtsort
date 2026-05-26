#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_MAX_H
#define _LLVM19___ALGORITHM_MAX_H

#include "llvm19_algorithm/comp.h"
#include "llvm19_algorithm/comp_ref_type.h"
#include "llvm19_algorithm/max_element.h"
#include "llvm19_config"
#include <initializer_list>

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

template <class _Tp, class _Compare>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 const _Tp&
max(_LLVM19_LIFETIMEBOUND const _Tp& __a, _LLVM19_LIFETIMEBOUND const _Tp& __b, _Compare __comp) {
  return __comp(__a, __b) ? __b : __a;
}

template <class _Tp>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 const _Tp&
max(_LLVM19_LIFETIMEBOUND const _Tp& __a, _LLVM19_LIFETIMEBOUND const _Tp& __b) {
  return target_std::max(__a, __b, __less<>());
}

#ifndef _LLVM19_CXX03_LANG

template <class _Tp, class _Compare>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 _Tp
max(initializer_list<_Tp> __t, _Compare __comp) {
  return *target_std::__max_element<__comp_ref_type<_Compare> >(__t.begin(), __t.end(), __comp);
}

template <class _Tp>
_LLVM19_NODISCARD inline _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 _Tp max(initializer_list<_Tp> __t) {
  return *target_std::max_element(__t.begin(), __t.end(), __less<>());
}

#endif // _LLVM19_CXX03_LANG

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_MAX_H
