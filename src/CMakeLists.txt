# Copyright (C) 2018 Michał Šrajer <michal@post.pl>
# This software is licensed under (one of your choice):
# - MIT
# - Apache-2.0
# - LGPL-2.1
# See LICENSE-*.txt for details.

cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")

add_executable (counts counts.cc)

find_package (Threads)
add_executable (benchmark benchmark.cc)
target_link_libraries (benchmark /usr/local/lib/libbenchmark.a ${CMAKE_THREAD_LIBS_INIT})
