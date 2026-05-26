#include "adapter.h"
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___FUNCTIONAL_RANGES_OPERATIONS_H
#define _LLVM15___FUNCTIONAL_RANGES_OPERATIONS_H

#include "llvm15_config"
#include "llvm15_utility/forward.h"
#include <concepts>

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

#if _LLVM15_STD_VER > 17

namespace ranges {

struct equal_to {
  template <class _Tp, class _Up>
  requires equality_comparable_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(_VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u)))) {
    return _VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u);
  }

  using is_transparent = void;
};

struct not_equal_to {
  template <class _Tp, class _Up>
  requires equality_comparable_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(!(_VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u))))) {
    return !(_VSTD::forward<_Tp>(__t) == _VSTD::forward<_Up>(__u));
  }

  using is_transparent = void;
};

struct less {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(_VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u)))) {
    return _VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u);
  }

  using is_transparent = void;
};

struct less_equal {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(!(_VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t))))) {
    return !(_VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t));
  }

  using is_transparent = void;
};

struct greater {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(_VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t)))) {
    return _VSTD::forward<_Up>(__u) < _VSTD::forward<_Tp>(__t);
  }

  using is_transparent = void;
};

struct greater_equal {
  template <class _Tp, class _Up>
  requires totally_ordered_with<_Tp, _Up>
  [[nodiscard]] constexpr bool operator()(_Tp &&__t, _Up &&__u) const
      noexcept(noexcept(bool(!(_VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u))))) {
    return !(_VSTD::forward<_Tp>(__t) < _VSTD::forward<_Up>(__u));
  }

  using is_transparent = void;
};

} // namespace ranges

#endif // _LLVM15_STD_VER > 17

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___FUNCTIONAL_RANGES_OPERATIONS_H
