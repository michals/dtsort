#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM19___ALGORITHM_COPY_MOVE_COMMON_H
#define _LLVM19___ALGORITHM_COPY_MOVE_COMMON_H

#include "llvm19_algorithm/iterator_operations.h"
#include "llvm19_algorithm/unwrap_iter.h"
#include "llvm19_algorithm/unwrap_range.h"
#include "llvm19_config"
#include "llvm19_iterator/iterator_traits.h"
#include "llvm19_memory/pointer_traits.h"
#include <__string/constexpr_c_functions.h>
#include "llvm19_type_traits/enable_if.h"
#include "llvm19_type_traits/is_always_bitcastable.h"
#include "llvm19_type_traits/is_constant_evaluated.h"
#include "llvm19_type_traits/is_constructible.h"
#include "llvm19_type_traits/is_trivially_assignable.h"
#include "llvm19_type_traits/is_volatile.h"
#include "llvm19_utility/move.h"
#include "llvm19_utility/pair.h"
#include <cstddef>

#if !defined(_LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM19_PUSH_MACROS
#include "llvm19_undef_macros"

_LLVM19_BEGIN_NAMESPACE_STD

// Type traits.

template <class _From, class _To>
struct __can_lower_copy_assignment_to_memmove {
  static const bool value =
      // If the types are always bitcastable, it's valid to do a bitwise copy between them.
      __is_always_bitcastable<_From, _To>::value &&
      // Reject conversions that wouldn't be performed by the regular built-in assignment (e.g. between arrays).
      is_trivially_assignable<_To&, const _From&>::value &&
      // `memmove` doesn't accept `volatile` pointers, make sure the optimization SFINAEs away in that case.
      !is_volatile<_From>::value && !is_volatile<_To>::value;
};

template <class _From, class _To>
struct __can_lower_move_assignment_to_memmove {
  static const bool value =
      __is_always_bitcastable<_From, _To>::value && is_trivially_assignable<_To&, _From&&>::value &&
      !is_volatile<_From>::value && !is_volatile<_To>::value;
};

// `memmove` algorithms implementation.

template <class _In, class _Out>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_In*, _Out*>
__copy_trivial_impl(_In* __first, _In* __last, _Out* __result) {
  const size_t __n = static_cast<size_t>(__last - __first);

  target_std::__constexpr_memmove(__result, __first, __element_count(__n));

  return target_std::make_pair(__last, __result + __n);
}

template <class _In, class _Out>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX14 pair<_In*, _Out*>
__copy_backward_trivial_impl(_In* __first, _In* __last, _Out* __result) {
  const size_t __n = static_cast<size_t>(__last - __first);
  __result -= __n;

  target_std::__constexpr_memmove(__result, __first, __element_count(__n));

  return target_std::make_pair(__last, __result);
}

// Iterator unwrapping and dispatching to the correct overload.

template <class _InIter, class _OutIter>
struct __can_rewrap
    : integral_constant<bool, is_copy_constructible<_InIter>::value && is_copy_constructible<_OutIter>::value> {};

template <class _Algorithm,
          class _InIter,
          class _Sent,
          class _OutIter,
          __enable_if_t<__can_rewrap<_InIter, _OutIter>::value, int> = 0>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX17 pair<_InIter, _OutIter>
__copy_move_unwrap_iters(_InIter __first, _Sent __last, _OutIter __out_first) {
  auto __range  = target_std::__unwrap_range(__first, target_std::move(__last));
  auto __result = _Algorithm()(target_std::move(__range.first), target_std::move(__range.second), target_std::__unwrap_iter(__out_first));
  return target_std::make_pair(target_std::__rewrap_range<_Sent>(target_std::move(__first), target_std::move(__result.first)),
                        target_std::__rewrap_iter(target_std::move(__out_first), target_std::move(__result.second)));
}

template <class _Algorithm,
          class _InIter,
          class _Sent,
          class _OutIter,
          __enable_if_t<!__can_rewrap<_InIter, _OutIter>::value, int> = 0>
_LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_SINCE_CXX17 pair<_InIter, _OutIter>
__copy_move_unwrap_iters(_InIter __first, _Sent __last, _OutIter __out_first) {
  return _Algorithm()(target_std::move(__first), target_std::move(__last), target_std::move(__out_first));
}

_LLVM19_END_NAMESPACE_STD

_LLVM19_POP_MACROS

#endif // _LLVM19___ALGORITHM_COPY_MOVE_COMMON_H
