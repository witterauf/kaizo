cmake_minimum_required(VERSION 3.12)
project(Fuse VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_DEBUG_POSTFIX d)
set(FUSE_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${FUSE_CMAKE_PATH})

enable_testing()
include(TargetWarnings)

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(python)