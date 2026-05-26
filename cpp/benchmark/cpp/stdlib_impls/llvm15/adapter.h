#pragma once

#include <algorithm>
#include <iterator>
#include <utility>
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>
#include <cstring> 

// Include dtsort infrastructure
#include "dt_dispatch.h"

// LLVM 15 Macros (Renamed to avoid conflict)
#define _LLVM15_BEGIN_NAMESPACE_STD namespace target_std {
#define _LLVM15_END_NAMESPACE_STD }
#define _LLVM15_HIDE_FROM_ABI
#define _LLVM15_CONSTEXPR constexpr
#define _LLVM15_CONSTEXPR_AFTER_CXX11 constexpr
#define _LLVM15_CONSTEXPR_AFTER_CXX14 constexpr
#define _LLVM15_CONSTEXPR_AFTER_CXX17 constexpr
#define _LLVM15_CONSTEXPR_AFTER_CXX20 constexpr
#define _LLVM15_INLINE_VISIBILITY inline
#define _LLVM15_NODISCARD [[nodiscard]]
#define _LLVM15_NODISCARD_EXT [[nodiscard]]
#define _LLVM15_DEPRECATED_IN_CXX20
#define _LLVM15_SUPPRESS_DEPRECATED_PUSH
#define _LLVM15_SUPPRESS_DEPRECATED_POP
#define _LLVM15_PUSH_MACROS
#define _LLVM15_POP_MACROS
#define _LLVM15_FUNC_VIS
#define _LLVM15_HIDDEN
#define _LLVM15_TEMPLATE_VIS
#define _LLVM15_DEPRECATED_IN_CXX11
#define _LLVM15_DEPRECATED_IN_CXX17
#define _LLVM15_DIAGNOSTIC_PUSH
#define _LLVM15_DIAGNOSTIC_POP
#define _LLVM15_CLANG_DIAGNOSTIC_IGNORED(x)
#define _LLVM15_HAS_NO_PRAGMA_SYSTEM_HEADER
#define _LLVM15_HARDENING_MODE_DEBUG 2
#define _LLVM15_HARDENING_MODE_NONE 0
#define _LLVM15_HARDENING_MODE _LLVM15_HARDENING_MODE_NONE

#define _VSTD target_std

namespace target_std {
    using std::size_t;
    using std::ptrdiff_t;
    using std::pair;
    using std::make_pair;
    using std::memmove;
    using std::memcpy;
    using std::forward;
    using std::swap;
    using std::iter_swap;
    using std::vector;
    using std::unique_ptr;
    using std::nothrow;
    using std::initializer_list; 
    
    using std::iterator_traits;
    using std::forward_iterator_tag;
    using std::bidirectional_iterator_tag;
    using std::random_access_iterator_tag;
    using std::input_iterator_tag;
    using std::advance;
    using std::distance;
    using std::next;
    using std::prev;
    using std::reverse_iterator; 
    template <class _Iter> using __unconstrained_reverse_iterator = std::reverse_iterator<_Iter>;
    
    using std::make_unsigned;
    using std::pointer_traits; 
    
    // Type traits
    using std::is_same;
    using std::is_trivially_copy_assignable;
    using std::is_reference;
    using std::enable_if;
    using std::enable_if_t;
    template <bool _Bp, class _Tp = void> using __enable_if_t = std::enable_if_t<_Bp, _Tp>; 

    using std::remove_reference;
    using std::remove_reference_t;
    using std::remove_const; 
    using std::remove_cv;
    using std::remove_cv_t;
    using std::integral_constant;
    using std::is_integral;
    using std::is_lvalue_reference;
    using std::conditional_t;
    using std::is_constructible;
    using std::is_copy_constructible;
    using std::is_nothrow_constructible;
    using std::is_copy_assignable;
    using std::declval;
    using std::is_nothrow_move_constructible;
    using std::is_nothrow_assignable;
    using std::is_nothrow_move_assignable;
    using std::is_trivially_copy_constructible;
    using std::is_trivially_move_assignable; 
    using std::is_trivially_copyable; 
    using std::is_arithmetic;
    using std::is_base_of;
    using std::true_type;
    using std::false_type;

    template <class _Tp> using __uncvref_t = std::remove_cv_t<std::remove_reference_t<_Tp>>;
    template <class _Iter> using __iter_value_type = typename std::iterator_traits<_Iter>::value_type;

    template <class _Tp>
    _LLVM15_NODISCARD _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR
    typename std::remove_reference<_Tp>::type&&
    move(_Tp&& __t) noexcept { return std::move(__t); }
    
    struct __identity {
        template <class _Tp>
        _LLVM15_CONSTEXPR _Tp&& operator()(_Tp&& __t) const noexcept {
            return std::forward<_Tp>(__t);
        }
    };
    
    struct __return_temporary_buffer {
        template <class _Tp> void operator()(_Tp* __p) const { ::operator delete(__p); }
    };

    template <class _Tp>
    pair<_Tp*, ptrdiff_t> get_temporary_buffer(ptrdiff_t __n) noexcept {
        if (__n > ptrdiff_t(-1) / sizeof(_Tp)) __n = ptrdiff_t(-1) / sizeof(_Tp);
        while (__n > 0) {
            void* __p = ::operator new(__n * sizeof(_Tp), std::nothrow);
            if (__p) return pair<_Tp*, ptrdiff_t>(static_cast<_Tp*>(__p), __n);
            __n /= 2;
        }
        return pair<_Tp*, ptrdiff_t>(nullptr, 0);
    }

    struct __destruct_n {
        ptrdiff_t __n_;
        explicit __destruct_n(ptrdiff_t __n) : __n_(__n) {}
        template <class _Tp> void __incr() { ++__n_; }
        template <class _Tp> void __set(ptrdiff_t __n, _Tp*) { __n_ = __n; }
        template <class _Tp> void operator()(_Tp* __p) const {
            for (ptrdiff_t __i = 0; __i < __n_; ++__i) __p[__i].~_Tp();
        }
    };

    inline int __libcpp_clz(unsigned int __x) { return __builtin_clz(__x); }
    inline int __libcpp_clz(unsigned long __x) { return __builtin_clzl(__x); }
    inline int __libcpp_clz(unsigned long long __x) { return __builtin_clzll(__x); }
    inline constexpr bool __libcpp_is_constant_evaluated() { return false; }
    template <class _Tp> constexpr auto __to_address(_Tp&& __t) noexcept { return std::to_address(std::forward<_Tp>(__t)); }
    
    template <class _Iter> struct __is_cpp17_contiguous_iterator : std::false_type {};
    template <class _Tp> struct __is_cpp17_contiguous_iterator<_Tp*> : std::true_type {};
    template <class _Iter> struct __is_cpp17_forward_iterator : std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<_Iter>::iterator_category> {};

    template <class _Fn, class... _Args>
    constexpr auto __invoke(_Fn&& __f, _Args&&... __args)
        noexcept(noexcept(std::invoke(std::forward<_Fn>(__f), std::forward<_Args>(__args)...)))
        -> decltype(std::invoke(std::forward<_Fn>(__f), std::forward<_Args>(__args)...))
    { return std::invoke(std::forward<_Fn>(__f), std::forward<_Args>(__args)...); }

    template <class _Fn, class... _Args> struct __is_callable : std::is_invocable<_Fn, _Args...> {};
    template <class _AlgPolicy, class _Iterator, class _Sentinel>
    _LLVM15_HIDE_FROM_ABI _LLVM15_CONSTEXPR_AFTER_CXX14
    void __debug_randomize_range(_Iterator, _Sentinel) {}

    template<typename _RandomAccessIterator, typename _Compare>
    inline void dtsort_dispatch_entry(_RandomAccessIterator __first, _RandomAccessIterator __last,
                                      _Compare __comp)
    {
        ::dt_dispatch(__first, __last, __comp);
    }
}

#ifndef _NOEXCEPT
#define _NOEXCEPT noexcept
#endif
