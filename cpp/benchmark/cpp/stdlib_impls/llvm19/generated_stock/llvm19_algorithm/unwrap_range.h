#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_UNWRAP_RANGE_H
#define _LLVM19___ALGORITHM_UNWRAP_RANGE_H

#include "llvm19_algorithm/unwrap_iter.h"
#include <__concepts/constructible.h>
#include "llvm19_config"
#include "llvm19_iterator/concepts.h"
#include "llvm19_iterator/next.h"
#include "llvm19_utility/declval.h"
#include "llvm19_utility/move.h"
#include "llvm19_utility/pair.h"

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

// __unwrap_range and __rewrap_range are used to unwrap ranges which may have different iterator and sentinel types.
// __unwrap_iter and __rewrap_iter don't work for this, because they assume that the iterator and sentinel have
// the same type. __unwrap_range tries to get two iterators and then forward to __unwrap_iter.

#if _LLVM19_STD_VER >= 20
template <class _Iter, class _Sent>
struct __unwrap_range_impl {
  _LLVM19_HIDE_FROM_ABI static constexpr auto __unwrap(_Iter __first, _Sent __sent)
    requires random_access_iterator<_Iter> && sized_sentinel_for<_Sent, _Iter>
  {
    auto __last = ranges::next(__first, __sent);
    return pair{target_std::__unwrap_iter(target_std::move(__first)), target_std::__unwrap_iter(target_std::move(__last))};
  }

  _LLVM19_HIDE_FROM_ABI static constexpr auto __unwrap(_Iter __first, _Sent __last) {
    return pair{target_std::move(__first), target_std::move(__last)};
  }

  _LLVM19_HIDE_FROM_ABI static constexpr auto
  __rewrap(_Iter __orig_iter, decltype(target_std::__unwrap_iter(target_std::move(__orig_iter))) __iter)
    requires random_access_iterator<_Iter> && sized_sentinel_for<_Sent, _Iter>
  {
    return target_std::__rewrap_iter(target_std::move(__orig_iter), target_std::move(__iter));
  }

  _LLVM19_HIDE_FROM_ABI static constexpr auto __rewrap(const _Iter&, _Iter __iter)
    requires(!(random_access_iterator<_Iter> && sized_sentinel_for<_Sent, _Iter>))
  {
    return __iter;
  }
};

template <class _Iter>
struct __unwrap_range_impl<_Iter, _Iter> {
  _LLVM19_HIDE_FROM_ABI static constexpr auto __unwrap(_Iter __first, _Iter __last) {
    return pair{target_std::__unwrap_iter(target_std::move(__first)), target_std::__unwrap_iter(target_std::move(__last))};
  }

  _LLVM19_HIDE_FROM_ABI static constexpr auto
  __rewrap(_Iter __orig_iter, decltype(target_std::__unwrap_iter(__orig_iter)) __iter) {
    return target_std::__rewrap_iter(target_std::move(__orig_iter), target_std::move(__iter));
  }
};

template <class _Iter, class _Sent>
_LLVM19_HIDE_FROM_ABI constexpr auto __unwrap_range(_Iter __first, _Sent __last) {
  return __unwrap_range_impl<_Iter, _Sent>::__unwrap(target_std::move(__first), target_std::move(__last));
}

template < class _Sent, class _Iter, class _Unwrapped>
_LLVM19_HIDE_FROM_ABI constexpr _Iter __rewrap_range(_Iter __orig_iter, _Unwrapped __iter) {
  return __unwrap_range_impl<_Iter, _Sent>::__rewrap(target_std::move(__orig_iter), target_std::move(__iter));
}
#else  // _LLVM19_STD_VER >= 20
template <class _Iter, class _Unwrapped = decltype(target_std::__unwrap_iter(target_std::declval<_Iter>()))>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR pair<_Unwrapped, _Unwrapped> __unwrap_range(_Iter __first, _Iter __last) {
  return target_std::make_pair(target_std::__unwrap_iter(target_std::move(__first)), target_std::__unwrap_iter(target_std::move(__last)));
}

template <class _Iter, class _Unwrapped>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR _Iter __rewrap_range(_Iter __orig_iter, _Unwrapped __iter) {
  return target_std::__rewrap_iter(target_std::move(__orig_iter), target_std::move(__iter));
}
#endif // _LLVM19_STD_VER >= 20

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_UNWRAP_RANGE_H
