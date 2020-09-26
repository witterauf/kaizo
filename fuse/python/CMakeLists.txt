cmake_minimum_required(VERSION 3.12)
project(fusepy)

#set_property(TARGET ... PROPERTY POSITION_INDEPENDENT_CODE ON)

# Not using PythonExtensions because it is only available when performing the build with
# scikit-build. However, development makes more sense with direct CMake usage.

find_package(Python COMPONENTS Development)

if (Python_FOUND)
  message(STATUS "Found Python ${Python_VERSION}")

  add_library(fusepy MODULE
    fusepy/src/fusepy.h
    fusepy/src/fusepy.cc
    fusepy/src/dataformat.h
    fusepy/src/dataformat.cc
  )
  target_compile_features(fusepy PRIVATE cxx_std_20)
  target_link_libraries(fusepy PRIVATE Fuse::Fuse Python::Module)
  if (WIN32)
    # Under Windows, Python >= 3.5 requires modules to be named .pyd, not .dll.
    set_target_properties(fusepy PROPERTIES SUFFIX ".pyd")
  else()
    set_target_properties(fusepy PROPERTIES PREFIX "")
  endif()

  install(TARGETS fusepy LIBRARY DESTINATION fusepy)
endif()