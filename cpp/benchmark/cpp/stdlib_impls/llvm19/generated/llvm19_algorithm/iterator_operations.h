#include "adapter.h"
#ifndef _LLVM19_PATCHED___ALGORITHM_ITERATOR_OPERATIONS_H
#define _LLVM19_PATCHED___ALGORITHM_ITERATOR_OPERATIONS_H

#include <iterator>
#include <utility>

_LLVM19_BEGIN_NAMESPACE_STD

struct _ClassicAlgPolicy {};
template <class _AlgPolicy> struct _IterOps;

template <>
struct _IterOps<_ClassicAlgPolicy> {
  template <class _Iter>
  using __value_type = typename target_std::iterator_traits<_Iter>::value_type;

  template <class _Iter>
  using __iterator_category = typename target_std::iterator_traits<_Iter>::iterator_category;

  template <class _Iter>
  using __difference_type = typename target_std::iterator_traits<_Iter>::difference_type;

  template <class _Iter, class _Distance>
  static void advance(_Iter& __iter, _Distance __count) { target_std::advance(__iter, __count); }

  template <class _Iter>
  static typename target_std::iterator_traits<_Iter>::difference_type distance(_Iter __first, _Iter __last) {
    return target_std::distance(__first, __last);
  }

  template <class _Iter>
  static auto&& __iter_move(_Iter&& __i) { return target_std::move(*target_std::forward<_Iter>(__i)); }

  template <class _Iter1, class _Iter2>
  static void iter_swap(_Iter1&& __a, _Iter2&& __b) { target_std::iter_swap(target_std::forward<_Iter1>(__a), target_std::forward<_Iter2>(__b)); }

  template <class _Iter>
  static _Iter next(_Iter __it, typename target_std::iterator_traits<_Iter>::difference_type __n = 1) {
    return target_std::next(__it, __n);
  }

  template <class _Iter>
  static _Iter prev(_Iter __it, typename target_std::iterator_traits<_Iter>::difference_type __n = 1) {
    return target_std::prev(__it, __n);
  }

  template <class _Iterator>
  static _Iterator next(_Iterator, _Iterator __last) { return __last; }
};

_LLVM19_END_NAMESPACE_STD

#endif
