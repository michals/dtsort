#include "adapter.h"
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___FUNCTIONAL_BINARY_FUNCTION_H
#define _LLVM15___FUNCTIONAL_BINARY_FUNCTION_H

#include "llvm15_config"

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

#if _LLVM15_STD_VER <= 14 || defined(_LLVM15_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)

template <class _Arg1, class _Arg2, class _Result>
struct _LLVM15_TEMPLATE_VIS _LLVM15_DEPRECATED_IN_CXX11 binary_function
{
    typedef _Arg1   first_argument_type;
    typedef _Arg2   second_argument_type;
    typedef _Result result_type;
};

#endif // _LLVM15_STD_VER <= 14 || defined(_LLVM15_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)

template <class _Arg1, class _Arg2, class _Result> struct __binary_function_keep_layout_base {
#if _LLVM15_STD_VER <= 17 || defined(_LLVM15_ENABLE_CXX20_REMOVED_BINDER_TYPEDEFS)
  using first_argument_type _LLVM15_DEPRECATED_IN_CXX17 = _Arg1;
  using second_argument_type _LLVM15_DEPRECATED_IN_CXX17 = _Arg2;
  using result_type _LLVM15_DEPRECATED_IN_CXX17 = _Result;
#endif
};

#if _LLVM15_STD_VER <= 14 || defined(_LLVM15_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)
_LLVM15_DIAGNOSTIC_PUSH
_LLVM15_CLANG_DIAGNOSTIC_IGNORED("-Wdeprecated-declarations")
template <class _Arg1, class _Arg2, class _Result>
using __binary_function = binary_function<_Arg1, _Arg2, _Result>;
_LLVM15_DIAGNOSTIC_POP
#else
template <class _Arg1, class _Arg2, class _Result>
using __binary_function = __binary_function_keep_layout_base<_Arg1, _Arg2, _Result>;
#endif

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___FUNCTIONAL_BINARY_FUNCTION_H
