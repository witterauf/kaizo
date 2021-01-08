cmake_minimum_required(VERSION 3.15)
project(kaizo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_DEBUG_POSTFIX d)
set(KAIZO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${KAIZO_CMAKE_PATH})

include(TargetWarnings)

add_subdirectory(external)
add_subdirectory(lib)
#add_subdirectory(python)