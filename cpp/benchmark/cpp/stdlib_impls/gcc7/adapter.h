#pragma once

// Comprehensive Shim layer for GCC 7
#include <iterator>
#include <functional>
#include <vector>
#include <memory>
#include <type_traits>
#include <bit>
#include <algorithm>
#include <utility>
#include <random>

// Generated dtsort headers

// Import dtsort dispatch
#include "dt_dispatch.hh"

// Avoid macro redefinitions for GCC 7
#ifdef __cpp_lib_clamp
#undef __cpp_lib_clamp
#endif
#ifdef __cpp_lib_sample
#undef __cpp_lib_sample
#endif

// GCC 7 Macros (stubs)
#define _GLIBCXX14_CONSTEXPR constexpr
#define _GLIBCXX17_CONSTEXPR constexpr
#define _GLIBCXX20_CONSTEXPR constexpr
#define _GLIBCXX_NODISCARD [[nodiscard]]
#define _GLIBCXX_TYPE_VIS
#define _GLIBCXX_VISIBILITY(x)
#define _GLIBCXX_BEGIN_NAMESPACE_VERSION
#define _GLIBCXX_END_NAMESPACE_VERSION
#define _GLIBCXX_ACCESSIBILITY(x)
#define _GLIBCXX_NOEXCEPT noexcept
#define _GLIBCXX_USE_NOEXCEPT noexcept

#define __glibcxx_function_requires(...)
#define __glibcxx_requires_valid_range(...)
#define __glibcxx_requires_irreflexive_pred(...)
#define __glibcxx_requires_irreflexive(...)
#define __glibcxx_requires_partitioned_pred(...)
#define __glibcxx_requires_partitioned_lower_pred(...)
#define __glibcxx_requires_partitioned_lower(...)
#define __glibcxx_requires_sorted_pred(...)
#define __glibcxx_requires_sorted(...)
#define __glibcxx_requires_sorted_set_pred(...)
#define __glibcxx_requires_sorted_set(...)
#define __glibcxx_requires_heap_pred(...)
#define __glibcxx_requires_heap(...)

#define _GLIBCXX_BEGIN_INLINE_ABI_NAMESPACE(x) inline namespace x {
#define _GLIBCXX_END_INLINE_ABI_NAMESPACE(x) }

#define _GLIBCXX_STD_A target_std

namespace target_std {
    using std::size_t;
    using std::ptrdiff_t;
    using std::forward_iterator_tag;
    using std::random_access_iterator_tag;
    using std::bidirectional_iterator_tag;
    using std::input_iterator_tag;
    using std::output_iterator_tag;
    using std::iterator_traits;
    using std::pair;
    using std::make_pair;
    using std::vector;
    using std::initializer_list;
    using std::swap;
    using std::iter_swap;
    using std::reverse_iterator;
    using std::distance;
    using std::advance;
    using std::move;
    using std::forward;
    using std::uniform_int_distribution;

    // Type Traits
    using std::is_same;
    using std::is_integral;
    using std::is_convertible;
    using std::make_unsigned;
    using std::make_unsigned_t;
    using std::remove_reference;
    using std::remove_reference_t;
    using std::common_type;
    using std::common_type_t;
    using std::enable_if;
    using std::enable_if_t;
    using std::is_base_of;
    using std::is_base_of_v;
    
    // GCC 7 specific traits
    template<typename... _Bn> struct __or_ : std::disjunction<_Bn...> {};
    template<typename... _Bn> struct __and_ : std::conjunction<_Bn...> {};
    template<typename _Pp> struct __not_ : std::negation<_Pp> {};

    // Internal name traits
    template<typename _Tp> using __make_unsigned = std::make_unsigned<_Tp>;
    template<typename _Tp> using __iterator_traits = std::iterator_traits<_Tp>;
    template<typename _Tp> using __uniform_int_distribution = std::uniform_int_distribution<_Tp>;

    // Algorithms (delegating to host std)
    template<typename... Args> auto min(Args&&... args) { return std::min(std::forward<Args>(args)...); }
    template<typename... Args> auto max(Args&&... args) { return std::max(std::forward<Args>(args)...); }
    template<typename... Args> auto min_element(Args&&... args) { return std::min_element(std::forward<Args>(args)...); }
    template<typename... Args> auto max_element(Args&&... args) { return std::max_element(std::forward<Args>(args)...); }
    template<typename... Args> auto minmax_element(Args&&... args) { return std::minmax_element(std::forward<Args>(args)...); }
    template<typename... Args> auto copy(Args&&... args) { return std::copy(std::forward<Args>(args)...); }
    template<typename... Args> auto copy_backward(Args&&... args) { return std::copy_backward(std::forward<Args>(args)...); }
    template<typename... Args> auto move_backward(Args&&... args) { return std::move_backward(std::forward<Args>(args)...); }
    template<typename... Args> auto fill(Args&&... args) { return std::fill(std::forward<Args>(args)...); }
    template<typename... Args> auto find_if(Args&&... args) { return std::find_if(std::forward<Args>(args)...); }
    template<typename... Args> auto find_if_not(Args&&... args) { return std::find_if_not(std::forward<Args>(args)...); }
    template<typename... Args> auto lower_bound(Args&&... args) { return std::lower_bound(std::forward<Args>(args)...); }
    template<typename... Args> auto upper_bound(Args&&... args) { return std::upper_bound(std::forward<Args>(args)...); }
    template<typename... Args> auto equal_range(Args&&... args) { return std::equal_range(std::forward<Args>(args)...); }
    template<typename... Args> auto binary_search(Args&&... args) { return std::binary_search(std::forward<Args>(args)...); }
    template<typename... Args> auto merge(Args&&... args) { return std::merge(std::forward<Args>(args)...); }
    template<typename... Args> auto inplace_merge(Args&&... args) { return std::inplace_merge(std::forward<Args>(args)...); }
    template<typename... Args> auto partition(Args&&... args) { return std::partition(std::forward<Args>(args)...); }
    template<typename... Args> auto stable_partition(Args&&... args) { return std::stable_partition(std::forward<Args>(args)...); }
    template<typename... Args> auto make_heap(Args&&... args) { return std::make_heap(std::forward<Args>(args)...); }
    template<typename... Args> auto push_heap(Args&&... args) { return std::push_heap(std::forward<Args>(args)...); }
    template<typename... Args> auto pop_heap(Args&&... args) { return std::pop_heap(std::forward<Args>(args)...); }
    template<typename... Args> auto sort_heap(Args&&... args) { return std::sort_heap(std::forward<Args>(args)...); }
    template<typename... Args> auto partial_sort(Args&&... args) { return std::partial_sort(std::forward<Args>(args)...); }
    template<typename... Args> auto partial_sort_copy(Args&&... args) { return std::partial_sort_copy(std::forward<Args>(args)...); }
    template<typename... Args> auto nth_element(Args&&... args) { return std::nth_element(std::forward<Args>(args)...); }
    template<typename... Args> auto unique_copy(Args&&... args) { return std::unique_copy(std::forward<Args>(args)...); }
    template<typename... Args> auto reverse(Args&&... args) { return std::reverse(std::forward<Args>(args)...); }
    template<typename... Args> auto rotate(Args&&... args) { return std::rotate(std::forward<Args>(args)...); }
    template<typename... Args> auto rotate_copy(Args&&... args) { return std::rotate_copy(std::forward<Args>(args)...); }
    template<typename... Args> auto swap_ranges(Args&&... args) { return std::swap_ranges(std::forward<Args>(args)...); }
    template<typename... Args> auto is_sorted_until(Args&&... args) { return std::is_sorted_until(std::forward<Args>(args)...); }

    // Real implementations for internal shims to avoid host resolution issues
    template<typename _ForwardIterator, typename _Tp, typename _Compare>
    _ForwardIterator __lower_bound(_ForwardIterator __first, _ForwardIterator __last,
                                   const _Tp& __val, _Compare __comp)
    {
        auto __len = std::distance(__first, __last);
        while (__len > 0) {
            auto __half = __len >> 1;
            auto __middle = __first;
            std::advance(__middle, __half);
            if (__comp(__middle, __val)) { 
                __first = __middle;
                ++__first;
                __len = __len - __half - 1;
            } else {
                __len = __half;
            }
        }
        return __first;
    }

    template<typename... Args> auto __is_permutation(Args&&... args) { return std::is_permutation(std::forward<Args>(args)...); }
    template<typename... Args> auto __count_if(Args&&... args) { return std::count_if(std::forward<Args>(args)...); }
    template<typename... Args> auto __make_heap(Args&&... args) { return std::make_heap(std::forward<Args>(args)...); }
    template<typename... Args> auto __pop_heap(Args&&... args) { return std::pop_heap(std::forward<Args>(args)...); }
    template<typename... Args> auto __sort_heap(Args&&... args) { return std::sort_heap(std::forward<Args>(args)...); }
    
    // Internal GCC helper name for heap adjustment
    template<typename _Iter, typename _Dist, typename _Ptr, typename _Comp>
    inline void __adjust_heap(_Iter __first, _Dist __holeIndex, _Dist __len, _Ptr __value, _Comp __comp) {
        // Mock
    }

    template<typename _Iter>
    inline _Iter __niter_base(_Iter __it) { return __it; }
    template<typename _Iter, typename _Orig>
    inline _Iter __niter_wrap(_Orig, _Iter __it) { return __it; }
    
    inline size_t __size_to_integer(size_t __n) { return __n; }

    template<typename _Iter>
    inline typename std::iterator_traits<_Iter>::iterator_category
    __iterator_category(const _Iter&) {
        return typename std::iterator_traits<_Iter>::iterator_category();
    }

    template<typename _ForwardIterator, typename _Tp>
    struct _Temporary_buffer {
        std::vector<_Tp> _M_buffer;
        ptrdiff_t _M_len;
        _Temporary_buffer(_ForwardIterator __first, _ForwardIterator __last) 
            : _M_buffer(std::distance(__first, __last)), _M_len(std::distance(__first, __last)) {}
        _Tp* begin() { return _M_buffer.data(); }
        ptrdiff_t size() const { return _M_len; }
        ptrdiff_t requested_size() const { return _M_len; }
    };

    inline int __lg(size_t __n) {
        if (__n == 0) return 0;
        return std::bit_width(__n) - 1;
    }
    
    template<typename _Tp> constexpr _Tp* __addressof(_Tp& __r) noexcept { return std::addressof(__r); }
    
    // Internal category checks
    template<typename _Tp> struct __is_byte { enum { __value = 0 }; };

    // Shims for internal names used by GCC headers
    template<typename... Args> auto __copy_move_a2(Args&&... args) { return std::copy(std::forward<Args>(args)...); }
    template<typename... Args> auto __copy_move_backward_a2(Args&&... args) { return std::copy_backward(std::forward<Args>(args)...); }
    template<typename... Args> auto __fill_a(Args&&... args) { return std::fill(std::forward<Args>(args)...); }

    // NOTE: __find_if, __search, __remove_if are defined in GCC 7 stl_algo.h, so we don't shim them here.

    // Mapping for dtsort-logic patch to original stock implementation
#define __insertion_sort_stock __insertion_sort
}

// Adapter entry point for dt_dispatch injection
template<typename _RandomAccessIterator, typename _Compare>
inline void dtsort_dispatch_entry(_RandomAccessIterator __first, _RandomAccessIterator __last,
                                  _Compare __comp)
{
    auto __wrapped_comp = [&](const auto& __a, const auto& __b) -> bool {
        return __comp(&__a, &__b);
    };
    ::dt_dispatch(__first, __last, __wrapped_comp);
}

#define _GLIBCXX_MOVE(x) std::move(x)
#define _GLIBCXX_MOVE3(a,b,c) std::move(a,b,c)
#define _GLIBCXX_MOVE_BACKWARD3(a,b,c) std::move_backward(a,b,c)
#define _GLIBCXX_BEGIN_NAMESPACE_ALGO
#define _GLIBCXX_END_NAMESPACE_ALGO 
