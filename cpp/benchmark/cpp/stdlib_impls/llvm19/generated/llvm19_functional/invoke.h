#include "adapter.h"
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___FUNCTIONAL_INVOKE_H
#define _LLVM19___FUNCTIONAL_INVOKE_H

#include "llvm19_config"
#include "llvm19_type_traits/invoke.h"
#include "llvm19_utility/forward.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_BEGIN_NAMESPACE_STD

#if _LLVM19_STD_VER >= 17

template <class _Fn, class... _Args>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX20 invoke_result_t<_Fn, _Args...>
invoke(_Fn&& __f, _Args&&... __args) noexcept(is_nothrow_invocable_v<_Fn, _Args...>) {
  return target_std::__invoke(target_std::forward<_Fn>(__f), target_std::forward<_Args>(__args)...);
}

#endif // _LLVM19_STD_VER >= 17

#if _LLVM19_STD_VER >= 23
template <class _Result, class _Fn, class... _Args>
  requires is_invocable_r_v<_Result, _Fn, _Args...>
_LLVM19_HIDE_FROM_ABI constexpr _Result
invoke_r(_Fn&& __f, _Args&&... __args) noexcept(is_nothrow_invocable_r_v<_Result, _Fn, _Args...>) {
  if constexpr (is_void_v<_Result>) {
    static_cast<void>(target_std::invoke(target_std::forward<_Fn>(__f), target_std::forward<_Args>(__args)...));
  } else {
    // TODO: Use reference_converts_from_temporary_v once implemented
    // using _ImplicitInvokeResult = invoke_result_t<_Fn, _Args...>;
    // static_assert(!reference_converts_from_temporary_v<_Result, _ImplicitInvokeResult>,
    static_assert(true,
                  "Returning from invoke_r would bind a temporary object to the reference return type, "
                  "which would result in a dangling reference.");
    return target_std::invoke(target_std::forward<_Fn>(__f), target_std::forward<_Args>(__args)...);
  }
}
#endif

_LLVM19_END_NAMESPACE_STD

#endif // _LLVM19___FUNCTIONAL_INVOKE_H
