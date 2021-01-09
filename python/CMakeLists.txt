cmake_minimum_required(VERSION 3.12)
project(kaizopy)

pybind11_add_module(kaizopy SHARED
  src/kaizopy/pyutilities.h
  src/kaizopy/pyutilities.cc
  src/kaizopy/kaizopy.h
  src/kaizopy/kaizopy.cc
  src/kaizopy/text.cc
  src/kaizopy/graphics.cc
  src/kaizopy/systems.cc
  src/kaizopy/addresses.cc
  src/kaizopy/dataformat.cc
  src/kaizopy/linking.cc
  src/kaizopy/data.h
  src/kaizopy/data.cc
)
target_compile_features(kaizopy PRIVATE cxx_std_20)
target_link_libraries(kaizopy
  PRIVATE
    Kaizo::Kaizo
)
#target_include_directories(kaizopy INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")
if (WIN32)
  target_compile_definitions(kaizopy PRIVATE "-DKAIZO_WIN_EXPORT")
endif()
#set_target_properties(kaizopy PROPERTIES DEBUG_POSTFIX "_d")

add_library(Kaizo::Python ALIAS kaizopy)

install(DIRECTORY kaizo DESTINATION python/)
install(TARGETS kaizopy
  LIBRARY
    DESTINATION python/kaizo/
  RUNTIME
    DESTINATION python/kaizo/
)
