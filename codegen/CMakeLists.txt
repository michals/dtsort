cmake_minimum_required(VERSION 3.14)
project(dtsort-codegen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Standalone dt_stable_sort library (header-only)
add_library(dt_stable_sort INTERFACE)
target_include_directories(dt_stable_sort INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../cpp/include
)
target_compile_features(dt_stable_sort INTERFACE cxx_std_17)

# Add executable from src/main.cc
add_executable(dtsort-codegen src/main.cc)
target_include_directories(dtsort-codegen PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Verification executable (tests and stats)
add_executable(dtsort-verify src/test_main.cc)
target_compile_definitions(dtsort-verify PRIVATE DT_ADAPTIVE=1)
target_include_directories(dtsort-verify PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Verification executable for High Entropy Mode (DT_ADAPTIVE=0)
add_executable(dtsort-verify-em src/test_main.cc)
target_compile_definitions(dtsort-verify-em PRIVATE DT_ADAPTIVE=0)
target_include_directories(dtsort-verify-em PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Verification executable for Adaptive Mode (DT_ADAPTIVE=1)
add_executable(dtsort-verify-am src/test_main.cc)
target_compile_definitions(dtsort-verify-am PRIVATE DT_ADAPTIVE=1)
target_include_directories(dtsort-verify-am PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Standalone stable sort test executable
add_executable(test-dt-stable-sort src/test_stable_sort.cc)
target_link_libraries(test-dt-stable-sort PRIVATE dt_stable_sort)
target_include_directories(test-dt-stable-sort PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)


# Enable warnings
if(MSVC)
    target_compile_options(dtsort-codegen PRIVATE /W4)
else()
    target_compile_options(dtsort-codegen PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(dtsort-verify PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(dtsort-verify-em PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(dtsort-verify-am PRIVATE -Wall -Wextra -pedantic)
endif()

# Testing
enable_testing()

# Internal Unit Tests
add_test(NAME UnitTests COMMAND dtsort-verify --unit-tests)
set_tests_properties(UnitTests PROPERTIES LABELS "unit")

# Statistical Verification Tests (N=2..9)
# Fast: Permutation sorting check only
# Full: Permutation + All N^N variations (duplicates) check
foreach(N RANGE 2 9)
    # Fast test (Permutations only)
    add_test(NAME VerifyN${N}_Fast COMMAND dtsort-verify --check -n ${N})
    set_tests_properties(VerifyN${N}_Fast PROPERTIES LABELS "fast")

    # Full test (includes Variations)
    # We only go up to N=8 for full variations in the standard suite to keep it reasonable.
    if(N LESS 9)
        add_test(NAME VerifyN${N}_Full COMMAND dtsort-verify --check -n ${N} --check-variations)
        set_tests_properties(VerifyN${N}_Full PROPERTIES LABELS "full")
    endif()
endforeach()

# Custom targets for ease of use
add_custom_target(test-fast
    COMMAND ${CMAKE_CTEST_COMMAND} -L fast --output-on-failure
    COMMENT "Running fast test suite (permutations only)"
)

add_custom_target(test-full
    COMMAND ${CMAKE_CTEST_COMMAND} -L full --output-on-failure
    COMMENT "Running full test suite (including all variations)"
)

add_custom_target(test-unit
    COMMAND ${CMAKE_CTEST_COMMAND} -L unit --output-on-failure
    COMMENT "Running internal unit tests"
)

add_custom_target(test-stable-sort
    COMMAND ./test-dt-stable-sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running dt_stable_sort correctness and stability tests"
    DEPENDS test-dt-stable-sort
)

# Adaptivity verification tests (High Entropy vs Adaptive Mode)
add_custom_target(test-adaptivity-em
    COMMAND $<TARGET_FILE:dtsort-verify-em> --check-all
    COMMENT "Running fast verification on High Entropy Mode (DT_ADAPTIVE=0)"
    DEPENDS dtsort-verify-em
)

add_custom_target(test-adaptivity-am
    COMMAND $<TARGET_FILE:dtsort-verify-am> --check-all
    COMMENT "Running fast verification on Adaptive Mode (DT_ADAPTIVE=1)"
    DEPENDS dtsort-verify-am
)

# Group-Sorted Decision Tree verification tests
set(GS_N_VALUES 6 7 8 9 10)
set(GS_MODE_VALUES gs42 gs52 gs53 gs54 gs55)
list(LENGTH GS_N_VALUES GS_COUNT)
math(EXPR GS_LAST "${GS_COUNT} - 1")
foreach(GS_IDX RANGE 0 ${GS_LAST})
    list(GET GS_N_VALUES ${GS_IDX} GS_N)
    list(GET GS_MODE_VALUES ${GS_IDX} GS_MODE)
    add_test(NAME VerifyN${GS_N}_${GS_MODE}_Fast
        COMMAND dtsort-verify --check -n ${GS_N} --mode ${GS_MODE})
    set_tests_properties(VerifyN${GS_N}_${GS_MODE}_Fast PROPERTIES LABELS "fast;gs")
endforeach()

add_custom_target(test-gs
    COMMAND ${CMAKE_CTEST_COMMAND} -L gs --output-on-failure
    COMMENT "Running group-sorted decision tree verification tests"
)

# New: All-in-one generation target
add_custom_target(generate
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/../cpp/output
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode full -n 2
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode full -n 3
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode full -n 4
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode full -n 5
    # GS Optimal variants
    # IMPORTANT: If you change the optimal GS configuration for any N, you MUST update:
    # 1. cpp/include/dt_stable_sort.hh (C++ includes and dispatch logic)
    # 2. codegen/CMakeLists.txt (this exact block below)
    # 3. cpp/benchmark/cpp/CMakeLists.txt (DT_MODE_GS file mappings)
    # 4. cpp/benchmark/runner/db.py (get_dt_sizes file mapping logic)
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode gs42 -n 6
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode gs52 -n 7
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode gs53 -n 8
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode gs54 -n 9
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/dtsort-codegen --mode gs55 -n 10
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../cpp
    COMMENT "Generating all dtsort headers and theoretical metrics in output/"
    DEPENDS dtsort-codegen dtsort-verify dtsort-verify-em dtsort-verify-am
)

add_custom_target(clean-generated
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/../cpp/output
    COMMENT "Removing all generated headers and metrics"
)

# Ensure test-dt-stable-sort waits for headers to be generated before compiling
add_dependencies(test-dt-stable-sort generate)

# Research and backtracking search targets
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/research")
    add_subdirectory(research)
endif()


