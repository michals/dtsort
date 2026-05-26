#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_MIN_H
#define _LLVM15___ALGORITHM_MIN_H

#include "llvm15_algorithm/comp.h"
#include "llvm15_algorithm/comp_ref_type.h"
#include "llvm15_algorithm/min_element.h"
#include "llvm15_config"
#include <initializer_list>

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_PUSH_MACROS
#include <__undef_macros>

_LLVM15_BEGIN_NAMESPACE_STD

template <class _Tp, class _Compare>
_LLVM15_NODISCARD_EXT inline
_LLVM15_INLINE_VISIBILITY _LLVM15_CONSTEXPR_AFTER_CXX11
const _Tp&
min(const _Tp& __a, const _Tp& __b, _Compare __comp)
{
    return __comp(__b, __a) ? __b : __a;
}

template <class _Tp>
_LLVM15_NODISCARD_EXT inline
_LLVM15_INLINE_VISIBILITY _LLVM15_CONSTEXPR_AFTER_CXX11
const _Tp&
min(const _Tp& __a, const _Tp& __b)
{
    return _VSTD::min(__a, __b, __less<_Tp>());
}

#ifndef _LLVM15_CXX03_LANG

template<class _Tp, class _Compare>
_LLVM15_NODISCARD_EXT inline
_LLVM15_INLINE_VISIBILITY _LLVM15_CONSTEXPR_AFTER_CXX11
_Tp
min(initializer_list<_Tp> __t, _Compare __comp)
{
    typedef typename __comp_ref_type<_Compare>::type _Comp_ref;
    return *_VSTD::__min_element<_Comp_ref>(__t.begin(), __t.end(), __comp);
}

template<class _Tp>
_LLVM15_NODISCARD_EXT inline
_LLVM15_INLINE_VISIBILITY _LLVM15_CONSTEXPR_AFTER_CXX11
_Tp
min(initializer_list<_Tp> __t)
{
    return *_VSTD::min_element(__t.begin(), __t.end(), __less<_Tp>());
}

#endif // _LLVM15_CXX03_LANG

_LLVM15_END_NAMESPACE_STD

_LLVM15_POP_MACROS

#endif // _LLVM15___ALGORITHM_MIN_H
