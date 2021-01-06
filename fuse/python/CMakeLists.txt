cmake_minimum_required(VERSION 3.12)
project(fusepy)

pybind11_add_module(fusepy SHARED
  src/fusepy/fusepy.h
  src/fusepy/fusepy.cc
  src/fusepy/addresses.h
  src/fusepy/addresses.cc
  src/fusepy/dataformat.h
  src/fusepy/dataformat.cc
  #src/fusepy/data.h
  #src/fusepy/data.cc
  src/fusepy/text.h
  src/fusepy/text.cc
  src/fusepy/binary.h
  src/fusepy/binary.cc
  #src/fusepy/objects.h
  #src/fusepy/objects.cc
  src/fusepy/linking.h
  src/fusepy/linking.cc
  #src/fusepy/python_helpers.h
  #src/fusepy/python_helpers.cc
)
target_compile_features(fusepy PRIVATE cxx_std_20)
target_link_libraries(fusepy PRIVATE Fuse::Fuse)
#set_target_properties(fusepy PROPERTIES DEBUG_POSTFIX "_d")

# For use by other Python C++ modules
if (WIN32)
  target_compile_definitions(fusepy PRIVATE "-DFUSE_WIN_EXPORT")
endif()
target_include_directories(fusepy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(Fuse::Python ALIAS fusepy)

install(DIRECTORY fuse DESTINATION python)
install(TARGETS fusepy
  LIBRARY
    DESTINATION python/fuse
  RUNTIME
    DESTINATION python/fuse
)