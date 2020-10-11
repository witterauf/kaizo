cmake_minimum_required(VERSION 3.12)
project(kaizopy)

find_package(Python COMPONENTS Development)

if (Python_FOUND)
  message(STATUS "Found Python ${Python_VERSION}")

  add_library(kaizopy SHARED
    src/kaizopy/kaizopy.h
    src/kaizopy/kaizopy.cc
    src/kaizopy/text.h
    src/kaizopy/text.cc
  )
  target_compile_features(kaizopy PRIVATE cxx_std_20)
  target_link_libraries(kaizopy
    PRIVATE
      Fuse::Fuse
      Kaizo::Kaizo
      Python::Module
  )
  target_compile_definitions(kaizopy PRIVATE "-DWIN_EXPORT")
  target_include_directories(kaizopy INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")
  if (WIN32)
    # Under Windows, Python >= 3.5 requires modules to be named .pyd, not .dll.
    set_target_properties(kaizopy PROPERTIES SUFFIX ".pyd")
  else()
    set_target_properties(kaizopy PROPERTIES PREFIX "")
  endif()
  set_target_properties(kaizopy PROPERTIES OUTPUT_NAME "_kaizopy")

  add_library(Kaizo::Python ALIAS kaizopy)

  install(DIRECTORY kaizopy DESTINATION python/)
  install(TARGETS kaizopy
    LIBRARY
      DESTINATION python/kaizopy/
    RUNTIME
      DESTINATION python/kaizopy/
  )
endif()