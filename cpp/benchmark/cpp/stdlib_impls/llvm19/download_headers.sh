#!/bin/bash
set -e

BASE_URL="https://raw.githubusercontent.com/llvm/llvm-project/release/19.x/libcxx/include"
DEST_DIR="benchmark/cpp/stdlib_impls/llvm19/original"

mkdir -p "$DEST_DIR/__algorithm"
mkdir -p "$DEST_DIR/__functional"
mkdir -p "$DEST_DIR/__debug_utils"
mkdir -p "$DEST_DIR/__bit"

function download() {
    local PATH_SUFFIX="$1"
    local URL="$BASE_URL/$PATH_SUFFIX"
    local OUT="$DEST_DIR/$PATH_SUFFIX"
    
    echo "Downloading $PATH_SUFFIX..."
    curl -f -s -o "$OUT" "$URL" || echo "Failed to download $PATH_SUFFIX"
}

# Core Algorithms
download "__algorithm/stable_sort.h"
download "__algorithm/sort.h"
download "__algorithm/inplace_merge.h"
download "__algorithm/comp.h"
download "__algorithm/comp_ref_type.h"
download "__algorithm/iterator_operations.h"
download "__algorithm/lower_bound.h"
download "__algorithm/upper_bound.h"
download "__algorithm/min.h"
download "__algorithm/max.h"
download "__algorithm/move.h"
download "__algorithm/move_backward.h"
download "__algorithm/rotate.h"
download "__algorithm/unwrap_iter.h"
download "__algorithm/unwrap_range.h" 
download "__algorithm/sift_down.h"
download "__algorithm/make_heap.h"
download "__algorithm/sort_heap.h"
download "__algorithm/push_heap.h"
download "__algorithm/pop_heap.h"
# download "__algorithm/iter_swap.h" - conflicts with using std::iter_swap
download "__algorithm/three_way_comp_ref_type.h"
download "__algorithm/swap_ranges.h"
download "__algorithm/min_element.h"
download "__algorithm/minmax.h"
download "__algorithm/copy.h"
download "__algorithm/copy_move_common.h"
download "__algorithm/copy_backward.h"
download "__algorithm/equal.h"
download "__algorithm/lexicographical_compare.h"
download "__algorithm/half_positive.h"
download "__algorithm/partial_sort.h"

# Debug utils - mocked in adapter
# download "__debug_utils/strict_weak_ordering_check.h"

# Bit operations - mocked in adapter
# download "__bit/countl.h"
# download "__bit/blsr.h"

# Functional
download "__functional/operations.h"
download "__functional/unary_function.h"
download "__functional/binary_function.h"
# download "__functional/identity.h" - conflicts with adapter __identity
download "__functional/invoke.h"

# NOTE: All other headers (__type_traits/*, __iterator/*, __utility/*,
#        __memory/*, __concepts/*) are auto-mocked by patch_script.sh.
#        The adapter includes system <type_traits>, <iterator>, <memory>, etc.
#        which provide all std:: symbols. Empty mock files satisfy #include
#        directives without pulling in LLVM's internal dependency graph.

echo "Download complete."
