#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_THREE_WAY_COMP_REF_TYPE_H
#define _LLVM19___ALGORITHM_THREE_WAY_COMP_REF_TYPE_H

#include <__assert>
#include <__compare/ordering.h>
#include "llvm19_config"
#include "llvm19_utility/declval.h"
#include "llvm19_utility/forward.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_BEGIN_NAMESPACE_STD

#if _LLVM19_STD_VER >= 20

template <class _Comp>
struct __debug_three_way_comp {
  _Comp& __comp_;
  _LLVM19_HIDE_FROM_ABI constexpr __debug_three_way_comp(_Comp& __c) : __comp_(__c) {}

  template <class _Tp, class _Up>
  _LLVM19_HIDE_FROM_ABI constexpr auto operator()(const _Tp& __x, const _Up& __y) {
    auto __r = __comp_(__x, __y);
    if constexpr (__comparison_category<decltype(__comp_(__x, __y))>)
      __do_compare_assert(__y, __x, __r);
    return __r;
  }

  template <class _Tp, class _Up>
  _LLVM19_HIDE_FROM_ABI constexpr auto operator()(_Tp& __x, _Up& __y) {
    auto __r = __comp_(__x, __y);
    if constexpr (__comparison_category<decltype(__comp_(__x, __y))>)
      __do_compare_assert(__y, __x, __r);
    return __r;
  }

  template <class _LHS, class _RHS, class _Order>
  _LLVM19_HIDE_FROM_ABI constexpr void __do_compare_assert(_LHS& __l, _RHS& __r, _Order __o) {
    _Order __expected = __o;
    if (__o == _Order::less)
      __expected = _Order::greater;
    if (__o == _Order::greater)
      __expected = _Order::less;
    _LLVM19_ASSERT_SEMANTIC_REQUIREMENT(
        __comp_(__l, __r) == __expected, "Comparator does not induce a strict weak ordering");
    (void)__l;
    (void)__r;
  }
};

// Pass the comparator by lvalue reference. Or in the debug mode, using a debugging wrapper that stores a reference.
#  if _LLVM19_HARDENING_MODE == _LLVM19_HARDENING_MODE_DEBUG
template <class _Comp>
using __three_way_comp_ref_type = __debug_three_way_comp<_Comp>;
#  else
template <class _Comp>
using __three_way_comp_ref_type = _Comp&;
#  endif

#endif // _LLVM19_STD_VER >= 20

_LLVM19_END_NAMESPACE_STD

#endif // _LLVM19___ALGORITHM_THREE_WAY_COMP_REF_TYPE_H
