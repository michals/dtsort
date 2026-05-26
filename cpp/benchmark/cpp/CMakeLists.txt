cmake_minimum_required(VERSION 3.16)
project(dtsort-benchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce Release build to ensure Google Benchmark works without warnings.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Maximum dtsort chunk size (5-10)
set(DTSORT_MAX_N 10 CACHE STRING "Maximum dtsort chunk size")
add_definitions(-DDT_MODE_GS)

# Set to OFF to skip GCC/LLVM patched stdlib targets (required on Windows;
# Unix shell patch scripts are not available there).
option(DTSORT_STDLIB_IMPLS "Build GCC/LLVM stdlib patched benchmark targets" ON)

# Include paths
set(BENCH_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DTSORT_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

include(FetchContent)
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
# Suppress benchmark installation and tests
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

# ============================================================================
# Helper function: creates one benchmark binary
# ============================================================================
function(add_bench_binary IMPL_ID IS_DTSORT IMPL_INCLUDE_DIR)
    if(IS_DTSORT)
        set(VARIANT "dtsort")
    else()
        set(VARIANT "stock")
    endif()

    set(TARGET_NAME "bench-${IMPL_ID}-${VARIANT}")

    add_executable(${TARGET_NAME} src/main.cc)

    # Compile definitions
    target_compile_definitions(${TARGET_NAME} PRIVATE
        IMPL_NAME="${IMPL_ID}"
        VARIANT="${VARIANT}"
        IS_STOCK=$<NOT:$<BOOL:${IS_DTSORT}>>
        MAX_DT_SIZE=${DTSORT_MAX_N}
        IMPL_${IMPL_ID}
    )

    if(IS_DTSORT)
        target_compile_definitions(${TARGET_NAME} PRIVATE VARIANT_DTSORT)
    endif()

    # Include directories
    target_include_directories(${TARGET_NAME} PRIVATE
        ${BENCH_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${DTSORT_GENERATED}
        ${IMPL_INCLUDE_DIR}
        ${IMPL_INCLUDE_DIR}/..
    )

    # Compile options (standardized; stdlib impl targets always use GCC/Clang)
    target_compile_options(${TARGET_NAME} PRIVATE
        -std=c++20 -Wall -Wextra -fpermissive
    )

    target_link_libraries(${TARGET_NAME} PRIVATE benchmark::benchmark)
endfunction()

# ============================================================================
# Declare binaries (incrementally)
# ============================================================================

# GCC/LLVM patched stdlib targets — require Unix shell patch scripts.
# Disabled automatically when DTSORT_STDLIB_IMPLS=OFF (e.g. on Windows or
# for DT-only benchmark runs).
if(DTSORT_STDLIB_IMPLS)

# GCC 14
add_bench_binary(GCC14 TRUE  "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/gcc14/generated")
add_bench_binary(GCC14 FALSE "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/gcc14/generated_stock")

# GCC 16
add_bench_binary(GCC16 TRUE  "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/gcc16/generated")
add_bench_binary(GCC16 FALSE "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/gcc16/generated_stock")

# GCC 7
add_bench_binary(GCC7 TRUE  "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/gcc7/generated")
add_bench_binary(GCC7 FALSE "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/gcc7/generated_stock")

# LLVM 15
add_bench_binary(LLVM15 TRUE  "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm15/generated;${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm15")
add_bench_binary(LLVM15 FALSE "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm15/generated_stock;${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm15")

# LLVM 19
add_bench_binary(LLVM19 TRUE  "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm19/generated;${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm19")
add_bench_binary(LLVM19 FALSE "${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm19/generated_stock;${CMAKE_CURRENT_SOURCE_DIR}/stdlib_impls/llvm19")

endif() # DTSORT_STDLIB_IMPLS

# Standalone dt_stable_sort (Stock, High Entropy Mode, Adaptive Mode)
foreach(VARIANT stock dtsort-em dtsort-am)
    set(TARGET_NAME "bench-dt_stable_sort-${VARIANT}")
    add_executable(${TARGET_NAME} src/main.cc)
    
    target_compile_definitions(${TARGET_NAME} PRIVATE
        IMPL_NAME="dt_stable_sort"
        VARIANT="${VARIANT}"
        MAX_DT_SIZE=${DTSORT_MAX_N}
        IMPL_DT_STABLE_SORT
    )
    
    if("${VARIANT}" STREQUAL "stock")
        target_compile_definitions(${TARGET_NAME} PRIVATE IS_STOCK=1)
    else()
        target_compile_definitions(${TARGET_NAME} PRIVATE IS_STOCK=0)
        if("${VARIANT}" STREQUAL "dtsort-em")
            target_compile_definitions(${TARGET_NAME} PRIVATE DT_ADAPTIVE=0)
        else()
            target_compile_definitions(${TARGET_NAME} PRIVATE DT_ADAPTIVE=1)
        endif()
    endif()

    target_include_directories(${TARGET_NAME} PRIVATE
        ${BENCH_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${DTSORT_GENERATED}
    )
    # DT-only targets: use platform-aware flags so Windows MSVC builds work.
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /std:c++20 /W3 /permissive-)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -std=c++20 -Wall -Wextra -fpermissive)
    endif()
    target_link_libraries(${TARGET_NAME} PRIVATE benchmark::benchmark)
endforeach()

# ============================================================================
# Per-N DT size objects
# Compile a tiny translation unit per N that only includes dtsortN.hh.
# This lets us measure the compiled object size in isolation for each N.
# ============================================================================
foreach(DT_N RANGE 2 ${DTSORT_MAX_N})
    set(SIZE_SRC "${CMAKE_CURRENT_BINARY_DIR}/dt_size_${DT_N}.cc")
    # GS mode: N<=5 use full, N=6+ use gs optimal
    # IMPORTANT: If you change the optimal GS configuration for any N, you MUST update:
    # 1. include/dt_stable_sort.hh (C++ includes and dispatch logic)
    # 2. CMakeLists.txt (root: generate target)
    # 3. benchmark/CMakeLists.txt (this exact block below)
    # 4. benchmark/runner/db.py (get_dt_sizes file mapping logic)
    if(${DT_N} EQUAL 6)
        set(DT_SUFFIX "_gs42")
        set(DT_FUNC_SUFFIX "gs42")
    elseif(${DT_N} EQUAL 7)
        set(DT_SUFFIX "_gs52")
        set(DT_FUNC_SUFFIX "gs52")
    elseif(${DT_N} EQUAL 8)
        set(DT_SUFFIX "_gs53")
        set(DT_FUNC_SUFFIX "gs53")
    elseif(${DT_N} EQUAL 9)
        set(DT_SUFFIX "_gs54")
        set(DT_FUNC_SUFFIX "gs54")
    elseif(${DT_N} EQUAL 10)
        set(DT_SUFFIX "_gs55")
        set(DT_FUNC_SUFFIX "gs55")
    else()
        set(DT_SUFFIX "_full")
        set(DT_FUNC_SUFFIX "")
    endif()

    # Explicit instantiation with int*/std::less<int> forces the compiler to emit
    # real decision-tree code. Without it the template produces an empty object.
    file(GENERATE OUTPUT ${SIZE_SRC}
        CONTENT "#include <functional>\n#include \"dtsort${DT_N}${DT_SUFFIX}.hh\"\n#if DT_ADAPTIVE\ntemplate bool dtsort${DT_N}${DT_FUNC_SUFFIX}<int*, std::less<int>>(int* const&, int* const&, const std::less<int>&);\n#else\ntemplate void dtsort${DT_N}${DT_FUNC_SUFFIX}<int*, std::less<int>>(int* const&, int* const&, const std::less<int>&);\n#endif\n"
    )
    add_library(dt_size_${DT_N}_obj OBJECT ${SIZE_SRC})
    target_include_directories(dt_size_${DT_N}_obj PRIVATE
        ${DTSORT_GENERATED}
        ${BENCH_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )
    if(MSVC)
        target_compile_options(dt_size_${DT_N}_obj PRIVATE /std:c++20 /O2)
    else()
        target_compile_options(dt_size_${DT_N}_obj PRIVATE -std=c++20 -O2)
    endif()
endforeach()
