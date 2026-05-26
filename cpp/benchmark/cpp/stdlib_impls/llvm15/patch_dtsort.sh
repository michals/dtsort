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
echo "Copying original LLVM 15 headers to unique names..."
mkdir -p "$GEN_DIR/llvm15_algorithm"
mkdir -p "$GEN_DIR/llvm15_functional"
cp -R "$ORIG_DIR/__algorithm/"* "$GEN_DIR/llvm15_algorithm/"
cp -R "$ORIG_DIR/__functional/"* "$GEN_DIR/llvm15_functional/"
cp "$ORIG_DIR/stable_sort.h" "$GEN_DIR/llvm15_stable_sort.h"

# Create stubs for missing internal headers
mkdir -p "$GEN_DIR/llvm15_iterator"
mkdir -p "$GEN_DIR/llvm15_utility"
mkdir -p "$GEN_DIR/llvm15_memory"
mkdir -p "$GEN_DIR/llvm15_type_traits"
mkdir -p "$GEN_DIR/llvm15_debug_utils"

touch "$GEN_DIR/llvm15_config"
touch "$GEN_DIR/llvm15_debug"
touch "$GEN_DIR/llvm15_bits"
touch "$GEN_DIR/llvm15_iterator/iterator_traits.h"
touch "$GEN_DIR/llvm15_iterator/advance.h"
touch "$GEN_DIR/llvm15_iterator/distance.h"
touch "$GEN_DIR/llvm15_iterator/next.h"
touch "$GEN_DIR/llvm15_iterator/prev.h"
touch "$GEN_DIR/llvm15_iterator/reverse_iterator.h"
touch "$GEN_DIR/llvm15_utility/move.h"
touch "$GEN_DIR/llvm15_utility/forward.h"
touch "$GEN_DIR/llvm15_utility/pair.h"
touch "$GEN_DIR/llvm15_utility/declval.h"
touch "$GEN_DIR/llvm15_memory/pointer_traits.h"
touch "$GEN_DIR/llvm15_type_traits/is_callable.h"
touch "$GEN_DIR/llvm15_type_traits/remove_reference.h"
touch "$GEN_DIR/llvm15_type_traits/is_copy_constructible.h"
touch "$GEN_DIR/llvm15_functional/identity.h"
touch "$GEN_DIR/llvm15_functional/invoke.h"
touch "$GEN_DIR/llvm15_debug_utils/randomize_range.h"

# Provide mock iterator_operations.h
cat > "$GEN_DIR/llvm15_algorithm/iterator_operations.h" <<EOF
#ifndef _LLVM15_PATCHED___ALGORITHM_ITERATOR_OPERATIONS_H
#define _LLVM15_PATCHED___ALGORITHM_ITERATOR_OPERATIONS_H

#include <iterator>
#include <utility>

_LLVM15_BEGIN_NAMESPACE_STD

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

_LLVM15_END_NAMESPACE_STD

#endif
EOF

# Patching function
function patch_file() {
    local FILE="$1"
    # Rename std:: to target_std:: (use word boundary to avoid target_target_std)
    sed -i '' 's/[[:<:]]std::/target_std::/g' "$FILE"
    # Rename _LIBCPP_ to _LLVM15_ (avoid overlapping prefix to prevent _PATCHED_PATCHED_)
    sed -i '' 's/_LIBCPP_/_LLVM15_/g' "$FILE"
    
    # Fix includes to use our unique names and quotes
    sed -i '' 's|<__algorithm/\([^>]*\)>|"llvm15_algorithm/\1"|g' "$FILE"
    sed -i '' 's|<__functional/\([^>]*\)>|"llvm15_functional/\1"|g' "$FILE"
    sed -i '' 's|<__iterator/\([^>]*\)>|"llvm15_iterator/\1"|g' "$FILE"
    sed -i '' 's|<__utility/\([^>]*\)>|"llvm15_utility/\1"|g' "$FILE"
    sed -i '' 's|<__memory/\([^>]*\)>|"llvm15_memory/\1"|g' "$FILE"
    sed -i '' 's|<__type_traits/\([^>]*\)>|"llvm15_type_traits/\1"|g' "$FILE"
    sed -i '' 's|<__config>|"llvm15_config"|g' "$FILE"
    sed -i '' 's|<__debug>|"llvm15_debug"|g' "$FILE"
    sed -i '' 's|<__bits>|"llvm15_bits"|g' "$FILE"
    sed -i '' 's|<__debug_utils/\([^>]*\)>|"llvm15_debug_utils/\1"|g' "$FILE"
    
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

echo "LLVM15 dtsort patching complete."
