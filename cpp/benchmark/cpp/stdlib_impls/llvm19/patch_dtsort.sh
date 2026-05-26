#!/bin/bash
set -e

# Setup directories
BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ORIG_DIR="$BASE_DIR/original"
GEN_DIR="$BASE_DIR/generated"

# Clean up old generated
rm -rf "$GEN_DIR"
mkdir -p "$GEN_DIR"

# Copy original files to unique names
echo "Copying original LLVM 19 headers to unique names..."
mkdir -p "$GEN_DIR/llvm19_algorithm"
mkdir -p "$GEN_DIR/llvm19_functional"
mkdir -p "$GEN_DIR/llvm19_debug_utils"
mkdir -p "$GEN_DIR/llvm19_bit"
cp -R "$ORIG_DIR/__algorithm/"* "$GEN_DIR/llvm19_algorithm/"
cp -R "$ORIG_DIR/__functional/"* "$GEN_DIR/llvm19_functional/"
cp -R "$ORIG_DIR/__debug_utils/"* "$GEN_DIR/llvm19_debug_utils/" || true
cp -R "$ORIG_DIR/__bit/"* "$GEN_DIR/llvm19_bit/" || true

# Create stubs for missing internal headers
mkdir -p "$GEN_DIR/llvm19_iterator"
mkdir -p "$GEN_DIR/llvm19_utility"
mkdir -p "$GEN_DIR/llvm19_memory"
mkdir -p "$GEN_DIR/llvm19_type_traits"
mkdir -p "$GEN_DIR/llvm19_bit"
for f in countl.h countr.h reverse.h rotate.h blsr.h countlz.h counttz.h popcount.h bit_cast.h bit_ceil.h bit_floor.h bit_width.h has_single_bit.h; do touch "$GEN_DIR/llvm19_bit/$f"; done

touch "$GEN_DIR/llvm19_config"
touch "$GEN_DIR/llvm19_debug"
touch "$GEN_DIR/llvm19_bits"
touch "$GEN_DIR/llvm19_undef_macros"

for f in iterator_traits.h advance.h distance.h next.h prev.h reverse_iterator.h iter_move.h iter_swap.h concepts.h segmented_iterator.h cpp17_iterator_concepts.h access.h data.h size.h empty.h; do touch "$GEN_DIR/llvm19_iterator/$f"; done
for f in move.h forward.h pair.h declval.h swap.h terminate.h exception_ptr.h; do touch "$GEN_DIR/llvm19_utility/$f"; done
for f in destruct_n.h temporary_buffer.h unique_ptr.h allocator_traits.h pointer_traits.h addressof.h voidify.h; do touch "$GEN_DIR/llvm19_memory/$f"; done
for f in is_trivially_assignable.h is_callable.h remove_reference.h is_copy_constructible.h desugars_to.h is_constant_evaluated.h integral_constant.h is_same.h enable_if.h conditional.h is_constructible.h is_nothrow_constructible.h is_copy_assignable.h is_trivially_copyable.h is_arithmetic.h type_identity.h is_integral.h is_floating_point.h make_unsigned.h is_enum.h invoke.h is_trivially_copy_assignable.h is_trivially_move_assignable.h is_trivially_copy_constructible.h is_trivially_move_constructible.h is_nothrow_assignable.h is_nothrow_copy_assignable.h is_nothrow_move_assignable.h is_always_bitcastable.h is_volatile.h is_const.h remove_const.h remove_volatile.h remove_cv.h common_type.h decay.h is_reference.h is_pointer.h is_scalar.h is_array.h underlying_type.h is_assignable.h is_move_assignable.h is_move_constructible.h is_nothrow_move_constructible.h is_trivially_default_constructible.h is_nothrow_default_constructible.h is_default_constructible.h disjunction.h conjunction.h negation.h; do touch "$GEN_DIR/llvm19_type_traits/$f"; done
for f in for_each_segment.h iter_swap.h; do touch "$GEN_DIR/llvm19_algorithm/$f"; done
for f in strict_weak_ordering_check.h randomize_range.h; do touch "$GEN_DIR/llvm19_debug_utils/$f"; done
for f in identity.h ranges_operations.h invoke.h bind.h function.h hash.h; do touch "$GEN_DIR/llvm19_functional/$f"; done

# Provide mock iterator_operations.h
cat > "$GEN_DIR/llvm19_algorithm/iterator_operations.h" <<EOF
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
  using __value_type = typename std::iterator_traits<_Iter>::value_type;

  template <class _Iter>
  using __iterator_category = typename std::iterator_traits<_Iter>::iterator_category;

  template <class _Iter>
  using __difference_type = typename std::iterator_traits<_Iter>::difference_type;

  template <class _Iter, class _Distance>
  static void advance(_Iter& __iter, _Distance __count) { std::advance(__iter, __count); }

  template <class _Iter>
  static typename std::iterator_traits<_Iter>::difference_type distance(_Iter __first, _Iter __last) {
    return std::distance(__first, __last);
  }

  template <class _Iter>
  static auto&& __iter_move(_Iter&& __i) { return std::move(*std::forward<_Iter>(__i)); }

  template <class _Iter1, class _Iter2>
  static void iter_swap(_Iter1&& __a, _Iter2&& __b) { std::iter_swap(std::forward<_Iter1>(__a), std::forward<_Iter2>(__b)); }

  template <class _Iter>
  static _Iter next(_Iter __it, typename std::iterator_traits<_Iter>::difference_type __n = 1) {
    return std::next(__it, __n);
  }

  template <class _Iter>
  static _Iter prev(_Iter __it, typename std::iterator_traits<_Iter>::difference_type __n = 1) {
    return std::prev(__it, __n);
  }

  template <class _Iterator>
  static _Iterator next(_Iterator, _Iterator __last) { return __last; }
};

_LLVM19_END_NAMESPACE_STD

#endif
EOF

# Patching function
function patch_file() {
    local FILE="$1"
    # Rename std:: to target_std::
    sed -i '' 's/[[:<:]]std::/target_std::/g' "$FILE"
    # Rename _LIBCPP_ to _LLVM19_
    sed -i '' 's/_LIBCPP_/_LLVM19_/g' "$FILE"
    
    # Fix includes to use our unique names and quotes
    sed -i '' 's|<__algorithm/\([^>]*\)>|"llvm19_algorithm/\1"|g' "$FILE"
    sed -i '' 's|<__functional/\([^>]*\)>|"llvm19_functional/\1"|g' "$FILE"
    sed -i '' 's|<__iterator/\([^>]*\)>|"llvm19_iterator/\1"|g' "$FILE"
    sed -i '' 's|<__utility/\([^>]*\)>|"llvm19_utility/\1"|g' "$FILE"
    sed -i '' 's|<__memory/\([^>]*\)>|"llvm19_memory/\1"|g' "$FILE"
    sed -i '' 's|<__type_traits/\([^>]*\)>|"llvm19_type_traits/\1"|g' "$FILE"
    sed -i '' 's|<__config>|"llvm19_config"|g' "$FILE"
    sed -i '' 's|<__debug>|"llvm19_debug"|g' "$FILE"
    sed -i '' 's|<__bits>|"llvm19_bits"|g' "$FILE"
    sed -i '' 's|<__bit/\([^>]*\)>|"llvm19_bit/\1"|g' "$FILE"
    sed -i '' 's|<__debug_utils/\([^>]*\)>|"llvm19_debug_utils/\1"|g' "$FILE"
    sed -i '' 's|<__undef_macros>|"llvm19_undef_macros"|g' "$FILE"
    
    # Add adapter include at the top if not present
    if ! grep -q "adapter.h" "$FILE"; then
        sed -i '' '1i\
#include "adapter.h"
' "$FILE"
    fi
}

# Apply patches to all generated files
find "$GEN_DIR" -type f -name "*.h" | while read f; do
    patch_file "$f"
done

# Apply algorithmic logic patches
echo "Applying dtsort logic patch..."
cd "$BASE_DIR"
patch -p0 < "patches/dtsort_logic.patch"

echo "LLVM19 dtsort patching complete."
