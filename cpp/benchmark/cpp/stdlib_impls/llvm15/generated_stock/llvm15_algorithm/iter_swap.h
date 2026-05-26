#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_ITER_SWAP_H
#define _LLVM15___ALGORITHM_ITER_SWAP_H

#include "llvm15_config"
#include "llvm15_utility/declval.h"
#include "llvm15_utility/swap.h"

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

template <class _ForwardIterator1, class _ForwardIterator2>
inline _LLVM15_INLINE_VISIBILITY _LLVM15_CONSTEXPR_AFTER_CXX17 void iter_swap(_ForwardIterator1 __a,
                                                                              _ForwardIterator2 __b)
    //                                  _NOEXCEPT_(_NOEXCEPT_(swap(*__a, *__b)))
    _NOEXCEPT_(_NOEXCEPT_(swap(*declval<_ForwardIterator1>(), *declval<_ForwardIterator2>()))) {
  swap(*__a, *__b);
}

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___ALGORITHM_ITER_SWAP_H
