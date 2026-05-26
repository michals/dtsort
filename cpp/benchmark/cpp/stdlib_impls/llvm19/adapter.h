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

#define _LLVM19_BEGIN_NAMESPACE_STD namespace target_std {
#define _LLVM19_END_NAMESPACE_STD }
#define _LLVM19_HIDE_FROM_ABI
#define _LLVM19_CTAD_SUPPORTED_FOR_TYPE(x)
#define _LLVM19_CONSTEXPR constexpr
#define _LLVM19_CONSTEXPR_AFTER_CXX11 constexpr
#define _LLVM19_CONSTEXPR_AFTER_CXX14 constexpr
#define _LLVM19_CONSTEXPR_AFTER_CXX17 constexpr
#define _LLVM19_CONSTEXPR_AFTER_CXX20 constexpr
#define _LLVM19_CONSTEXPR_SINCE_CXX11 constexpr
#define _LLVM19_CONSTEXPR_SINCE_CXX14 constexpr
#define _LLVM19_CONSTEXPR_SINCE_CXX17 constexpr
#define _LLVM19_CONSTEXPR_SINCE_CXX20 constexpr
#define _LLVM19_CONSTEXPR_SINCE_CXX23 constexpr
#define _LLVM19_INLINE_VISIBILITY inline
#define _LLVM19_NODISCARD [[nodiscard]]
#define _LLVM19_NODISCARD_EXT [[nodiscard]]
#define _LLVM19_DEPRECATED_IN_CXX20
#define _LLVM19_SUPPRESS_DEPRECATED_PUSH
#define _LLVM19_SUPPRESS_DEPRECATED_POP
#define _LLVM19_PUSH_MACROS
#define _LLVM19_POP_MACROS
#define _LLVM19_FUNC_VIS
#define _LLVM19_HIDDEN
#define _LLVM19_TEMPLATE_VIS
#define _LLVM19_DEPRECATED_IN_CXX11
#define _LLVM19_DEPRECATED_IN_CXX17
#define _LLVM19_DIAGNOSTIC_PUSH
#define _LLVM19_DIAGNOSTIC_POP
#define _LLVM19_CLANG_DIAGNOSTIC_IGNORED(x)
#define _LLVM19_HAS_NO_PRAGMA_SYSTEM_HEADER
#define _LLVM19_HARDENING_MODE_DEBUG 2
#define _LLVM19_HARDENING_MODE_NONE 0
#define _LLVM19_HARDENING_MODE _LLVM19_HARDENING_MODE_NONE
#define _LLVM19_LIFETIMEBOUND
#define _LLVM19_EXPORTED_FROM_ABI
#define _LLVM19_VISIBILITY_ALL
#define _LLVM19_ASSERT_INTERNAL(x, m) ((void)0)
#define _LLVM19_ASSERT_SEMANTIC_REQUIREMENT(x, m) ((void)0)
#define _LLVM19_ASSERT_VALID_ELEMENT_ACCESS(x, m) ((void)0)
#define _LLVM19_ASSERT_VALID_ITERATOR_RANGE(x, m) ((void)0)
#define _LLVM19_ASSERT_VALID_ALGORITHM_INPUT(x, m) ((void)0)
#define _LLVM19_ASSERT_COMPARTMENT_IS_OPEN() ((void)0)
#define _LLVM19_ASSERT_NON_OVERLAPPING_RANGES(x, m) ((void)0)
#define _LLVM19_EXTERN_VIS
#define _LLVM19_ALWAYS_INLINE __attribute__((__always_inline__))
#define _LLVM19_DEVICE
#define _LLVM19_HOST

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
    using std::is_floating_point;
    using std::is_volatile;
    using std::is_const;
    using std::is_array;
    using std::is_scalar;
    using std::is_pointer;
    using std::is_reference;
    using std::is_assignable;
    using std::is_constructible;
    using std::is_copy_constructible;
    using std::is_move_assignable;
    using std::is_move_constructible;
    using std::is_trivially_assignable;
    using std::is_trivially_copyable; 
    using std::is_arithmetic;
    using std::is_enum;
    using std::is_base_of;
    using std::is_lvalue_reference;
    using std::is_copy_assignable;
    using std::is_nothrow_constructible;
    using std::is_nothrow_assignable;
    using std::is_nothrow_copy_assignable;
    using std::is_nothrow_move_assignable;
    using std::is_nothrow_move_constructible;
    using std::is_trivially_constructible;
    using std::is_trivially_copy_constructible;
    using std::is_trivially_move_constructible;
    using std::is_trivially_move_assignable; 
    using std::conditional;
    using std::conditional_t;
    using std::declval;

    using std::true_type;
    using std::false_type;
    using std::common_type;
    template <class... _Tp> using common_type_t = std::common_type_t<_Tp...>;
    using std::decay;
    template <class _Tp> using decay_t = std::decay_t<_Tp>;
    using std::make_unsigned;
    template <class _Tp> using __make_unsigned_t = std::make_unsigned_t<_Tp>;

    // Logical traits
    using std::disjunction;
    using std::conjunction;
    using std::negation;
    template <class... _Args> using __disjunction_t = std::disjunction<_Args...>;
    template <class... _Args> using __conjunction_t = std::conjunction<_Args...>;
    template <class _Arg> using __negation_t = std::negation<_Arg>;
    template <class... _Args> using _Or = std::disjunction<_Args...>;
    template <class... _Args> using _And = std::conjunction<_Args...>;
    template <class _Arg> using _Not = std::negation<_Arg>;

    // Tags and traits for desugars_to
    struct __plus_tag {};
    struct __minus_tag {};
    struct __multiplies_tag {};
    struct __divides_tag {};
    struct __modulus_tag {};
    struct __negate_tag {};
    struct __bit_and_tag {};
    struct __bit_or_tag {};
    struct __bit_xor_tag {};
    struct __bit_not_tag {};
    struct __equal_tag {};
    struct __not_equal_tag {};
    struct __less_tag {};
    struct __less_equal_tag {};
    struct __greater_tag {};
    struct __greater_equal_tag {};
    struct __logical_and_tag {};
    struct __logical_or_tag {};
    struct __logical_not_tag {};
    struct __identity_tag {};

    template <class _Tag, class _Functor, class _Tp, class _Up>
    inline constexpr bool __desugars_to_v = false;
    template <class _Iter> using __has_forward_iterator_category = std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<_Iter>::iterator_category>;
    template <class _Iter> using __has_random_access_iterator_category = std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<_Iter>::iterator_category>;
    template <class _Iter> using __libcpp_is_contiguous_iterator = std::is_pointer<_Iter>;

    template <class _Tp> using __uncvref_t = std::remove_cv_t<std::remove_reference_t<_Tp>>;
    template <class _Iter> using __iter_value_type = typename std::iterator_traits<_Iter>::value_type;
    template <class _Iter> using __iter_diff_t = typename std::iterator_traits<_Iter>::difference_type;

    // Segmented iterator support (minimal)
    template <class _Iter> struct __is_segmented_iterator : std::false_type {};
    template <class _Iter> struct __segmented_iterator_traits;

    template <class _Iter, class _Functor>
    _LLVM19_HIDE_FROM_ABI void __for_each_segment(_Iter __first, _Iter __last, _Functor __func) {
        // Simple non-segmented implementation
        __func(__first, __last);
    }

    template <class _RandomAccessIterator, class _Comp>
    _LLVM19_HIDE_FROM_ABI void __check_strict_weak_ordering_sorted(_RandomAccessIterator, _RandomAccessIterator, _Comp&) {
        // Debug check, no-op for benchmarks
    }

    // Bitcastable trait
    template <class _From, class _To>
    struct __is_always_bitcastable : std::integral_constant<bool, sizeof(_From) == sizeof(_To) && std::is_trivially_copyable<_From>::value && std::is_trivially_copyable<_To>::value> {};

    // Element count and memory operations
    enum class __element_count : size_t {};

    template <class _Tp, class _Up>
    static inline _LLVM19_HIDE_FROM_ABI void __constexpr_memmove(_Tp* __dest, _Up* __src, __element_count __n) {
        std::memmove(__dest, __src, static_cast<size_t>(__n));
    }

    template <class _Tp, class _Up>
    static inline _LLVM19_HIDE_FROM_ABI void __constexpr_copy_aligned(_Tp* __dest, _Up* __src, __element_count __n) {
        std::copy(__src, __src + static_cast<size_t>(__n), __dest);
    }

    template <class _Tp>
    _LLVM19_NODISCARD _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR
    typename std::remove_reference<_Tp>::type&&
    move(_Tp&& __t) noexcept { return std::move(__t); }
    
    struct __identity {
        template <class _Tp>
        _LLVM19_CONSTEXPR _Tp&& operator()(_Tp&& __t) const noexcept {
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
    inline int __libcpp_ctz(unsigned long long __x) { return __builtin_ctzll(__x); }
    _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR unsigned long long __libcpp_blsr(unsigned long long __x) { return __x & (__x - 1); }
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
    _LLVM19_HIDE_FROM_ABI _LLVM19_CONSTEXPR_AFTER_CXX14
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
