cmake_minimum_required(VERSION 3.12)
project(LinColn VERSION 1.0 LANGUAGES CXX)

add_executable(make-collection
  CLI11.hpp
  Main.cc
  MakeCollection.h
  MakeCollection.cc
)
target_compile_features(make-collection PRIVATE cxx_std_17)

macro(expect ARGUMENT ERROR_MESSAGE)
  if (NOT ${ARGUMENT})
    message(FATAL_ERROR ${ERROR_MESSAGE})
  endif()
endmacro(expect)

macro(default ARGUMENT DEFAULT)
  if (NOT ${ARGUMENT})
    set(${ARGUMENT} ${DEFAULT})
  endif()
endmacro(default)

function(make_collection COLLECTION_NAME)
  # parse arguments
  set(OPTION_ARGS DEBUG)
  set(ONE_VALUE_ARGS WORKING_DIRECTORY OUTPUT_DIRECTORY NAMESPACE)
  set(MULTI_VALUE_ARGS FILES)
  cmake_parse_arguments(PARSE_ARGV 1 COLLECTION
    "${OPTION_ARGS}"
    "${ONE_VALUE_ARGS}"
    "${MULTI_VALUE_ARGS}"
  )
  expect(COLLECTION_FILES "make_collection(): expected argument FILES")
  default(COLLECTION_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BUILD_DIRECTORY}")
  
  if (COLLECTION_DEBUG)
    message(STATUS "make_collection(): Files: ${COLLECTION_FILES}")
    message(STATUS "make_collection(): Namespace: ${COLLECTION_NAMESPACE}")
    message(STATUS "make_collection(): Working directory: ${COLLECTION_WORKING_DIRECTORY}")
    message(STATUS "make_collection(): Output directory: ${COLLECTION_OUTPUT_DIRECTORY}")
  endif()

  if (NOT COLLECTION_OUTPUT_DIRECTORY)
    set(COLLECTION_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endif()

  set(COLLECTION_COMMANDLINE "--collection ${COLLECTION_NAME} --output-directory ${COLLECTION_OUTPUT_DIRECTORY}")
  if (COLLECTION_WORKING_DIRECTORY)
    set(COLLECTION_COMMANDLINE "${COLLECTION_COMMANDLINE} --directory ${COLLECTION_WORKING_DIRECTORY}")
  endif()
  if (COLLECTION_NAMESPACE)
    set(COLLECTION_COMMANDLINE "${COLLECTION_COMMANDLINE} --namespace \"${COLLECTION_NAMESPACE}\"")
  endif()

  set(COLLECTION_OUTPUTS
    "${COLLECTION_OUTPUT_DIRECTORY}/${COLLECTION_NAME}.cc"
    "${COLLECTION_OUTPUT_DIRECTORY}/${COLLECTION_NAME}.h"
  )
  set("${COLLECTION_NAME}_SOURCES" "${COLLECTION_OUTPUTS}" PARENT_SCOPE)

  string(JOIN " " COLLECTION_COMMANDLINE_INPUTS ${COLLECTION_FILES})
  set(COLLECTION_COMMANDLINE "${COLLECTION_COMMANDLINE} ${COLLECTION_COMMANDLINE_INPUTS}")
  
  if (COLLECTION_DEBUG)
    message(STATUS "make_collection(): Commandline: ${COLLECTION_COMMANDLINE}")
    message(STATUS "make_collection(): Outputs: ${COLLECTION_OUTPUTS}")
  endif()

  if (NOT TARGET make-collection)
    message(FATAL_ERROR "make_collection(): make-collection is not a target")
  endif()
  
  add_custom_command(
    OUTPUT ${COLLECTION_OUTPUTS}
    DEPENDS ${COLLECTION_FILES} make-collection
    COMMAND make-collection ${COLLECTION_COMMANDLINE}
    COMMENT "Building collection ${COLLECTION_NAME}..."
  )
endfunction(make_collection)

function(add_collection_library COLLECTION_NAME)
  set(MULTI_ARGS FILES)
  set(SINGLE_ARGS WORKING_DIRECTORY OUTPUT_DIRECTORY NAMESPACE)
  cmake_parse_arguments(PARSE_ARGV 1 COLLECTION
    ""
    "${SINGLE_ARGS}"
    "${MULTI_ARGS}"
  )  
  expect(COLLECTION_FILES "add_collection_library(): expected argument FILES")
  default(COLLECTION_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BUILD_DIRECTORY}")
  
  make_collection(${COLLECTION_NAME}
    FILES ${COLLECTION_FILES}
    WORKING_DIRECTORY ${COLLECTION_WORKING_DIRECTORY}
    OUTPUT_DIRECTORY ${COLLECTION_OUTPUT_DIRECTORY}
    NAMESPACE ${COLLECTION_NAMESPACE}
    DEBUG
  )
  
  add_library(${COLLECTION_NAME} STATIC
    ${${COLLECTION_NAME}_SOURCES}
  )
  target_compile_features(${COLLECTION_NAME} PUBLIC cxx_std_17)
  target_include_directories(${COLLECTION_NAME} PUBLIC ${COLLECTION_OUTPUT_DIRECTORY})
endfunction(add_collection_library)