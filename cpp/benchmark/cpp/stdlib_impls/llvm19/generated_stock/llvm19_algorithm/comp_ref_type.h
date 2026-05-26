#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_COMP_REF_TYPE_H
#define _LLVM19___ALGORITHM_COMP_REF_TYPE_H

#include <__assert>
#include "llvm19_config"
#include "llvm19_utility/declval.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_BEGIN_NAMESPACE_STD

template <class _Compare>
struct __debug_less {
  _Compare& __comp_;
  _LLVM19_CONSTEXPR_SINCE_CXX14 _LLVM19_HIDE_FROM_ABI __debug_less(_Compare& __c) : __comp_(__c) {}

  template <class _Tp, class _Up>
  _LLVM19_CONSTEXPR_SINCE_CXX14 _LLVM19_HIDE_FROM_ABI bool operator()(const _Tp& __x, const _Up& __y) {
    bool __r = __comp_(__x, __y);
    if (__r)
      __do_compare_assert(0, __y, __x);
    return __r;
  }

  template <class _Tp, class _Up>
  _LLVM19_CONSTEXPR_SINCE_CXX14 _LLVM19_HIDE_FROM_ABI bool operator()(_Tp& __x, _Up& __y) {
    bool __r = __comp_(__x, __y);
    if (__r)
      __do_compare_assert(0, __y, __x);
    return __r;
  }

  template <class _LHS, class _RHS>
  _LLVM19_CONSTEXPR_SINCE_CXX14 inline
      _LLVM19_HIDE_FROM_ABI decltype((void)target_std::declval<_Compare&>()(target_std::declval<_LHS&>(), target_std::declval<_RHS&>()))
      __do_compare_assert(int, _LHS& __l, _RHS& __r) {
    _LLVM19_ASSERT_SEMANTIC_REQUIREMENT(!__comp_(__l, __r), "Comparator does not induce a strict weak ordering");
    (void)__l;
    (void)__r;
  }

  template <class _LHS, class _RHS>
  _LLVM19_CONSTEXPR_SINCE_CXX14 inline _LLVM19_HIDE_FROM_ABI void __do_compare_assert(long, _LHS&, _RHS&) {}
};

// Pass the comparator by lvalue reference. Or in the debug mode, using a debugging wrapper that stores a reference.
#if _LLVM19_HARDENING_MODE == _LLVM19_HARDENING_MODE_DEBUG
template <class _Comp>
using __comp_ref_type = __debug_less<_Comp>;
#else
template <class _Comp>
using __comp_ref_type = _Comp&;
#endif

_LLVM19_END_NAMESPACE_STD

#endif // _LLVM19___ALGORITHM_COMP_REF_TYPE_H
