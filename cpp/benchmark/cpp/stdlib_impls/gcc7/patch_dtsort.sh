#!/bin/bash
set -e
BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GEN_DIR="$BASE_DIR/generated"
rm -rf "$GEN_DIR"
mkdir -p "$GEN_DIR"
mkdir -p "$GEN_DIR/bits"
mkdir -p "$GEN_DIR/ext"

# Copy files
cp "$BASE_DIR/original/stl_algo.h" "$GEN_DIR/stl_algo.h"
cp "$BASE_DIR/original/bits/predefined_ops.h" "$GEN_DIR/bits/predefined_ops.h"

# Create minimal stubs for missing dependencies
touch "$GEN_DIR/bits/stl_algobase.h" 
touch "$GEN_DIR/bits/algorithmfwd.h"
touch "$GEN_DIR/bits/stl_heap.h"
touch "$GEN_DIR/bits/stl_tempbuf.h" 
touch "$GEN_DIR/bits/stl_construct.h"
touch "$GEN_DIR/bits/uniform_int_dist.h"
touch "$GEN_DIR/bits/c++config.h"
touch "$GEN_DIR/bits/stl_relops.h"
touch "$GEN_DIR/bits/cpp_type_traits.h"
touch "$GEN_DIR/bits/os_defines.h"
touch "$GEN_DIR/bits/cpu_defines.h"
touch "$GEN_DIR/bits/functexcept.h"
touch "$GEN_DIR/bits/move.h"
touch "$GEN_DIR/ext/type_traits.h"
touch "$GEN_DIR/ext/numeric_traits.h"

ALG_FILE="$GEN_DIR/stl_algo.h"
PREDEF_FILE="$GEN_DIR/bits/predefined_ops.h"

# Inject adapter.h
sed -i '' '/#define _STL_ALGO_H 1/a\
#include "../adapter.h"\
' "$ALG_FILE"

# Aggressive renaming std -> target_std
for f in "$ALG_FILE" "$PREDEF_FILE"; do
    sed -i '' 's/namespace std/namespace target_std/g' "$f"
    sed -i '' 's/} \/\/ namespace std/} \/\/ namespace target_std/g' "$f"
    sed -i '' 's/std::/target_std::/g' "$f"
    # Fix internal includes to use quotes
    sed -i '' 's/#include <bits\/\(.*\)>/#include "bits\/\1"/' "$f"
    sed -i '' 's/#include <ext\/\(.*\)>/#include "ext\/\1"/' "$f"
done

# Apply algorithmic logic patches
echo "Applying dtsort logic patch..."
cd "$BASE_DIR"
patch -p0 < "patches/dtsort_logic.patch"

echo "GCC7 dtsort patching complete."
