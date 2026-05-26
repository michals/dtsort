#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_COMP_H
#define _LLVM19___ALGORITHM_COMP_H

#include "llvm19_config"
#include "llvm19_type_traits/desugars_to.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_BEGIN_NAMESPACE_STD

struct __equal_to {
  template <class _T1, class _T2>
  _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 bool operator()(const _T1& __x, const _T2& __y) const {
    return __x == __y;
  }
};

template <class _Tp, class _Up>
inline const bool __desugars_to_v<__equal_tag, __equal_to, _Tp, _Up> = true;

// The definition is required because __less is part of the ABI, but it's empty
// because all comparisons should be transparent.
template <class _T1 = void, class _T2 = _T1>
struct __less {};

template <>
struct __less<void, void> {
  template <class _Tp, class _Up>
  _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 bool operator()(const _Tp& __lhs, const _Up& __rhs) const {
    return __lhs < __rhs;
  }
};

template <class _Tp>
inline const bool __desugars_to_v<__less_tag, __less<>, _Tp, _Tp> = true;

_LLVM19_END_NAMESPACE_STD

#endif // _LLVM19___ALGORITHM_COMP_H
