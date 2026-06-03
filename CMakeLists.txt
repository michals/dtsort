cmake_minimum_required(VERSION 3.16)
project(dtsort-root NONE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Include subprojects for workspace-wide build and IntelliSense
add_subdirectory(codegen)
add_subdirectory(cpp/examples)
add_subdirectory(cpp/benchmark/cpp)
