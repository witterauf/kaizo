cmake_minimum_required(VERSION 3.12)
project(fusepy)

#set_property(TARGET ... PROPERTY POSITION_INDEPENDENT_CODE ON)

# Not using PythonExtensions because it is only available when performing the build with
# scikit-build. However, development makes more sense with direct CMake usage.

find_package(Python COMPONENTS Development)

if (Python_FOUND)
  message(STATUS "Found Python ${Python_VERSION}")

  add_library(fusepy SHARED
    src/fusepy/fusepy.h
    src/fusepy/fusepy.cc
    src/fusepy/dataformat.h
    src/fusepy/dataformat.cc
    src/fusepy/data.h
    src/fusepy/data.cc
    src/fusepy/addresses.h
    src/fusepy/addresses.cc
    src/fusepy/text.h
    src/fusepy/text.cc
    src/fusepy/binary.h
    src/fusepy/binary.cc
    src/fusepy/python_helpers.h
    src/fusepy/python_helpers.cc
  )
  target_compile_features(fusepy PRIVATE cxx_std_20)
  target_link_libraries(fusepy PRIVATE Fuse::Fuse Python::Module)
  target_include_directories(fusepy INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  if (WIN32)
    target_compile_definitions(fusepy PRIVATE "-DWIN_EXPORT")
    # Under Windows, Python >= 3.5 requires modules to be named .pyd, not .dll.
    set_target_properties(fusepy PROPERTIES SUFFIX ".pyd")
  else()
    set_target_properties(fusepy PROPERTIES PREFIX "")
  endif()
  set_target_properties(fusepy PROPERTIES DEBUG_POSTFIX "_d")
  set_target_properties(fusepy PROPERTIES OUTPUT_NAME "_fusepy")
  add_library(Fuse::Python ALIAS fusepy)

  install(DIRECTORY fusepy DESTINATION python)
  install(TARGETS fusepy
    LIBRARY
      DESTINATION python/fusepy/ # e.g. Linux
    RUNTIME
      DESTINATION python/fusepy/ # Windows
  )
endif()