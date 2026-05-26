cmake_minimum_required(VERSION 3.12)
project(dtsort-examples CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simple_sort simple_sort.cc)
add_executable(simple_benchmark simple_benchmark.cc)

# Include directories pointing to the dtsort interface and generated sorting kernels
target_include_directories(simple_sort PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../output
)
target_include_directories(simple_benchmark PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../output
)

