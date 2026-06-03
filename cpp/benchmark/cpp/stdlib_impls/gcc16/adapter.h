#pragma once

// Comprehensive Shim layer for GCC 16
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

// GCC 16 Macros (stubs)
#define _GLIBCXX_HOSTED 1
#define _GLIBCXX14_CONSTEXPR constexpr
#define _GLIBCXX17_CONSTEXPR constexpr
#define _GLIBCXX20_CONSTEXPR constexpr
#define _GLIBCXX_CONSTEXPR constexpr
#define _GLIBCXX26_CONSTEXPR
#define __glibcxx_constexpr_algorithms 0
#define _GLIBCXX_NODISCARD [[nodiscard]]
#define _GLIBCXX_TYPE_VIS
#define _GLIBCXX_VISIBILITY(x)
#define _GLIBCXX_BEGIN_NAMESPACE_VERSION
#define _GLIBCXX_END_NAMESPACE_VERSION
#define _GLIBCXX_ACCESSIBILITY(x)
#define _GLIBCXX_NOEXCEPT noexcept
#define _GLIBCXX_USE_NOEXCEPT noexcept

#define __glibcxx_function_requires(...)
#define _GLIBCXX_MOVE(__val) std::move(__val)
#define _GLIBCXX_MOVE3(__first, __last, __result) std::move(__first, __last, __result)
#define _GLIBCXX_MOVE_BACKWARD3(__first, __last, __result) std::move_backward(__first, __last, __result)
#define _GLIBCXX_FORWARD(_Tp, __val) std::forward<_Tp>(__val)

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
#define __no_unique_address__ no_unique_address

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
    using std::less;
    using std::equal_to;
    using std::swap;
    using std::iter_swap;
    using std::reverse_iterator;
    using std::distance;
    using std::advance;
    using std::move;
    using std::forward;
    using std::shared_ptr;
    using std::unique_ptr;
    using std::make_unique;
    using std::make_shared;
    using std::nullptr_t;
    using std::uniform_int_distribution;

    // Type Traits
    using std::is_same;
    using std::is_integral;
    using std::is_convertible;
    using std::make_unsigned;
    using std::make_unsigned_t;
    using std::remove_reference;
    using std::remove_reference_t;
    using std::is_empty;
    using std::is_scalar;
    using std::common_type;
    using std::common_type_t;
    using std::enable_if;
    using std::enable_if_t;
    using std::is_base_of;
    using std::is_base_of_v;
    
    // Internal name traits
    template<typename _Tp> using __make_unsigned = std::make_unsigned<_Tp>;
    template<typename _Tp> using __iterator_traits = std::iterator_traits<_Tp>;
    template<typename _Tp> using __uniform_int_distribution = std::uniform_int_distribution<_Tp>;
    template<bool _Cond, typename _If, typename _Then> using __conditional_t = std::conditional_t<_Cond, _If, _Then>;
    template<typename... _Bn> using __or_ = std::bool_constant<(std::is_base_of_v<std::true_type, _Bn> || ...)>;

    // Algorithms (delegating to host std)
    template<typename... Args> auto min(Args&&... args) { return std::min(std::forward<Args>(args)...); }
    template<typename... Args> auto max(Args&&... args) { return std::max(std::forward<Args>(args)...); }
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
    template<typename... Args> auto __lower_bound(Args&&... args) { return std::lower_bound(std::forward<Args>(args)...); }

    template<typename _ForwardIterator1, typename _ForwardIterator2, typename _BinaryPredicate>
    _ForwardIterator1 __search(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
                               _ForwardIterator2 __first2, _ForwardIterator2 __last2,
                               _BinaryPredicate __p)
    {
        return std::search(__first1, __last1, __first2, __last2, __p);
    }

    template<typename _ForwardIterator, typename _Predicate>
    _ForwardIterator __remove_if(_ForwardIterator __first, _ForwardIterator __last, _Predicate __p)
    {
        return std::remove_if(__first, __last, __p);
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

    template<typename _Iter>
    inline auto __iter_concept_or_category(const _Iter& __it) {
        return __iterator_category(__it);
    }
    
    template<typename _Iter>
    using __iter_concept_or_category_t = decltype(__iterator_category(std::declval<_Iter>()));

    template<typename _ForwardIterator, typename _Tp>
    struct _Temporary_buffer {
        _Tp* _M_buffer;
        ptrdiff_t _M_len;
        ptrdiff_t _M_requested_len;
        
        template<typename _Arg2>
        _Temporary_buffer(_ForwardIterator __first, _Arg2 __arg2) {
            if constexpr (std::is_integral_v<_Arg2> || std::is_enum_v<_Arg2>) {
                _M_requested_len = static_cast<ptrdiff_t>(__arg2);
            } else {
                _M_requested_len = std::distance(__first, __arg2);
            }
            _M_len = _M_requested_len;
            _M_buffer = nullptr;
            if (_M_len > 0) {
                _M_buffer = static_cast<_Tp*>(::operator new(_M_len * sizeof(_Tp), std::nothrow));
                if (!_M_buffer) _M_len = 0;
            }
        }
        
        ~_Temporary_buffer() {
            if (_M_buffer) ::operator delete(_M_buffer);
        }
        
        _Tp* begin() { return _M_buffer; }
        ptrdiff_t size() const { return _M_len; }
        ptrdiff_t requested_size() const { return _M_requested_len; }
        ptrdiff_t _M_requested_size() const { return _M_requested_len; }
    };

    inline int __lg(size_t __n) {
        if (__n == 0) return 0;
        return std::bit_width(__n) - 1;
    }

    template<typename _Iter, typename _Pred>
    inline _Iter __find_if(_Iter __f, _Iter __l, _Pred __p) {
        return std::find_if(__f, __l, __p);
    }
    
    template<typename _Tp> constexpr _Tp* __addressof(_Tp& __r) noexcept { return std::addressof(__r); }
    
    // Internal category checks
    template<typename _Tp> struct __is_byte { enum { __value = 0 }; };

    // Shims for internal names used by GCC headers
    template<typename... Args> auto __copy_move_a2(Args&&... args) { return std::copy(std::forward<Args>(args)...); }
    template<typename... Args> auto __copy_move_backward_a2(Args&&... args) { return std::copy_backward(std::forward<Args>(args)...); }
    template<typename... Args> auto __fill_a(Args&&... args) { return std::fill(std::forward<Args>(args)...); }
    template<typename... Args> auto __copy_n_a(Args&&... args) { return std::copy_n(std::forward<Args>(args)...); }

    // Memory construction shims
    template<typename _T1, typename... _Args>
    inline void _Construct(_T1* __p, _Args&&... __args) {
        ::new(static_cast<void*>(__p)) _T1(std::forward<_Args>(__args)...);
    }

    template<typename _Tp>
    inline void _Destroy(_Tp* __p) {
        if (__p) __p->~_Tp();
    }

    template<typename _InputIterator, typename _ForwardIterator, typename _Alloc>
    inline _ForwardIterator __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
                                                   _ForwardIterator __result, _Alloc& __alloc) {
        return std::uninitialized_copy(__first, __last, __result);
    }

    template<typename _InputIterator, typename _ForwardIterator, typename _Alloc>
    inline _ForwardIterator __uninitialized_move_a(_InputIterator __first, _InputIterator __last,
                                                   _ForwardIterator __result, _Alloc& __alloc) {
        return std::uninitialized_move(__first, __last, __result);
    }

    using __true_type = std::true_type;
    using __false_type = std::false_type;

    template<bool _Val> struct __truth_type { typedef __false_type __type; };
    template<> struct __truth_type<true> { typedef __true_type __type; };
}


template<typename _RandomAccessIterator, typename _Compare>
inline void dtsort_dispatch_entry(_RandomAccessIterator __first, _RandomAccessIterator __last,
                                  _Compare __comp)
{
    auto __wrapped_comp = [&](auto& __a, auto& __b) -> bool {
        return bool(__comp(__a, __b));
    };
    ::dt_dispatch(__first, __last, __wrapped_comp);
}


#define _GLIBCXX_BEGIN_NAMESPACE_ALGO
#define _GLIBCXX_END_NAMESPACE_ALGO
