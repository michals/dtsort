#include "adapter.h"
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LLVM15___ALGORITHM_MOVE_H
#define _LLVM15___ALGORITHM_MOVE_H

#include "llvm15_algorithm/iterator_operations.h"
#include "llvm15_algorithm/unwrap_iter.h"
#include "llvm15_config"
#include "llvm15_iterator/iterator_traits.h"
#include "llvm15_iterator/reverse_iterator.h"
#include "llvm15_utility/move.h"
#include "llvm15_utility/pair.h"
#include <cstring>
#include <type_traits>

#if !defined(_LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LLVM15_BEGIN_NAMESPACE_STD

// move

template <class _AlgPolicy, class _InIter, class _Sent, class _OutIter>
inline _LLVM15_INLINE_VISIBILITY _LLVM15_CONSTEXPR_AFTER_CXX14
pair<_InIter, _OutIter> __move_impl(_InIter __first, _Sent __last, _OutIter __result) {
  while (__first != __last) {
    *__result = _IterOps<_AlgPolicy>::__iter_move(__first);
    ++__first;
    ++__result;
  }
  return target_std::make_pair(target_std::move(__first), target_std::move(__result));
}

template <class _AlgPolicy,
          class _InType,
          class _OutType,
          class = __enable_if_t<is_same<typename remove_const<_InType>::type, _OutType>::value
                             && is_trivially_move_assignable<_OutType>::value> >
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
pair<_InType*, _OutType*> __move_impl(_InType* __first, _InType* __last, _OutType* __result) {
  if (__libcpp_is_constant_evaluated()
// TODO: Remove this once GCC supports __builtin_memmove during constant evaluation
#ifndef _LLVM15_COMPILER_GCC
   && !is_trivially_copyable<_InType>::value
#endif
     )
    return target_std::__move_impl<_AlgPolicy, _InType*, _InType*, _OutType*>(__first, __last, __result);
  const size_t __n = static_cast<size_t>(__last - __first);
  ::__builtin_memmove(__result, __first, __n * sizeof(_OutType));
  return target_std::make_pair(__first + __n, __result + __n);
}

template <class>
struct __is_trivially_move_assignable_unwrapped_impl : false_type {};

template <class _Type>
struct __is_trivially_move_assignable_unwrapped_impl<_Type*> : is_trivially_move_assignable<_Type> {};

template <class _Iter>
struct __is_trivially_move_assignable_unwrapped
    : __is_trivially_move_assignable_unwrapped_impl<decltype(target_std::__unwrap_iter<_Iter>(target_std::declval<_Iter>()))> {};

template <class _AlgPolicy,
          class _InIter,
          class _OutIter,
          __enable_if_t<is_same<typename remove_const<typename iterator_traits<_InIter>::value_type>::type,
                                typename iterator_traits<_OutIter>::value_type>::value
                     && __is_cpp17_contiguous_iterator<_InIter>::value
                     && __is_cpp17_contiguous_iterator<_OutIter>::value
                     && is_trivially_move_assignable<__iter_value_type<_OutIter> >::value, int> = 0>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX14
pair<reverse_iterator<_InIter>, reverse_iterator<_OutIter> >
__move_impl(reverse_iterator<_InIter> __first,
            reverse_iterator<_InIter> __last,
            reverse_iterator<_OutIter> __result) {
  auto __first_base = target_std::__unwrap_iter(__first.base());
  auto __last_base = target_std::__unwrap_iter(__last.base());
  auto __result_base = target_std::__unwrap_iter(__result.base());
  auto __result_first = __result_base - (__first_base - __last_base);
  target_std::__move_impl<_AlgPolicy>(__last_base, __first_base, __result_first);
  return target_std::make_pair(__last, reverse_iterator<_OutIter>(target_std::__rewrap_iter(__result.base(), __result_first)));
}

template <class _AlgPolicy, class _InIter, class _Sent, class _OutIter>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
__enable_if_t<is_copy_constructible<_InIter>::value
           && is_copy_constructible<_Sent>::value
           && is_copy_constructible<_OutIter>::value, pair<_InIter, _OutIter> >
__move(_InIter __first, _Sent __last, _OutIter __result) {
  auto __ret = target_std::__move_impl<_AlgPolicy>(
      target_std::__unwrap_iter(__first), target_std::__unwrap_iter(__last), target_std::__unwrap_iter(__result));
  return target_std::make_pair(target_std::__rewrap_iter(__first, __ret.first), target_std::__rewrap_iter(__result, __ret.second));
}

template <class _AlgPolicy, class _InIter, class _Sent, class _OutIter>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX11
__enable_if_t<!is_copy_constructible<_InIter>::value
           || !is_copy_constructible<_Sent>::value
           || !is_copy_constructible<_OutIter>::value, pair<_InIter, _OutIter> >
__move(_InIter __first, _Sent __last, _OutIter __result) {
  return target_std::__move_impl<_AlgPolicy>(target_std::move(__first), target_std::move(__last), target_std::move(__result));
}

template <class _InputIterator, class _OutputIterator>
inline _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX17
_OutputIterator move(_InputIterator __first, _InputIterator __last, _OutputIterator __result) {
  return target_std::__move<_ClassicAlgPolicy>(__first, __last, __result).second;
}

_LLVM15_END_NAMESPACE_STD

#endif // _LLVM15___ALGORITHM_MOVE_H
