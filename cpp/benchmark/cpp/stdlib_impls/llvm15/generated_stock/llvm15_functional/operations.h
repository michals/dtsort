#include "adapter.h"
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___FUNCTIONAL_OPERATIONS_H
#define _LLVM15___FUNCTIONAL_OPERATIONS_H

#include "llvm15_config"
#include "llvm15_functional/binary_function.h"
#include "llvm15_functional/unary_function.h"
#include "llvm15_utility/forward.h"

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

// Arithmetic operations

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS plus
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x + __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS plus<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) + _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) + _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) + _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS minus
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x - __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS minus<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) - _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) - _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) - _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS multiplies
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x * __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS multiplies<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) * _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) * _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) * _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS divides
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x / __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS divides<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) / _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) / _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) / _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS modulus
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x % __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS modulus<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) % _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) % _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) % _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS negate
    : __unary_function<_Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x) const
        {return -__x;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS negate<void>
{
    template <class _Tp>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_Tp&& __x) const
        noexcept(noexcept(- _VSTD::forward<_Tp>(__x)))
        -> decltype(      - _VSTD::forward<_Tp>(__x))
        { return          - _VSTD::forward<_Tp>(__x); }
    typedef void is_transparent;
};
#endif

// Bitwise operations

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS bit_and
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x & __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS bit_and<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) & _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) & _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) & _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
struct _LLVM15_TEMPLATE_VIS bit_not
    : __unary_function<_Tp, _Tp>
{
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x) const
        {return ~__x;}
};

template <>
struct _LLVM15_TEMPLATE_VIS bit_not<void>
{
    template <class _Tp>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_Tp&& __x) const
        noexcept(noexcept(~_VSTD::forward<_Tp>(__x)))
        -> decltype(      ~_VSTD::forward<_Tp>(__x))
        { return          ~_VSTD::forward<_Tp>(__x); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS bit_or
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x | __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS bit_or<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) | _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) | _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) | _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS bit_xor
    : __binary_function<_Tp, _Tp, _Tp>
{
    typedef _Tp __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x ^ __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS bit_xor<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) ^ _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) ^ _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) ^ _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

// Comparison operations

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS equal_to
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x == __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS equal_to<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) == _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) == _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) == _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS not_equal_to
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x != __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS not_equal_to<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) != _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) != _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) != _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS less
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x < __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS less<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) < _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) < _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) < _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS less_equal
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x <= __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS less_equal<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) <= _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) <= _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) <= _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS greater_equal
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x >= __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS greater_equal<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) >= _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) >= _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) >= _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS greater
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x > __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS greater<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) > _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) > _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) > _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

// Logical operations

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS logical_and
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x && __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS logical_and<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) && _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) && _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) && _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS logical_not
    : __unary_function<_Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x) const
        {return !__x;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS logical_not<void>
{
    template <class _Tp>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_Tp&& __x) const
        noexcept(noexcept(!_VSTD::forward<_Tp>(__x)))
        -> decltype(      !_VSTD::forward<_Tp>(__x))
        { return          !_VSTD::forward<_Tp>(__x); }
    typedef void is_transparent;
};
#endif

#if _LLVM15_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LLVM15_TEMPLATE_VIS logical_or
    : __binary_function<_Tp, _Tp, bool>
{
    typedef bool __result_type;  // used by valarray
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x || __y;}
};

#if _LLVM15_STD_VER > 11
template <>
struct _LLVM15_TEMPLATE_VIS logical_or<void>
{
    template <class _T1, class _T2>
    _LLVM15_CONSTEXPR_AFTER_CXX11 _LLVM15_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) || _VSTD::forward<_T2>(__u)))
        -> decltype(      _VSTD::forward<_T1>(__t) || _VSTD::forward<_T2>(__u))
        { return          _VSTD::forward<_T1>(__t) || _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___FUNCTIONAL_OPERATIONS_H
