add_library(lua STATIC
  src/lapi.c
  src/lauxlib.c
  src/lbaselib.c
  src/lbitlib.c
  src/lcode.c
  src/lcorolib.c
  src/lctype.c
  src/ldblib.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/linit.c
  src/liolib.c
  src/llex.c
  src/lmathlib.c
  src/lmem.c
  src/loadlib.c
  src/lobject.c
  src/lopcodes.c
  src/loslib.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/lstrlib.c
  src/ltable.c
  src/ltablib.c
  src/ltm.c
  src/lua.c
  src/lundump.c
  src/lutf8lib.c
  src/lvm.c
  src/lzio.c
)
target_include_directories(lua PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_library(Lua::Library ALIAS lua)

add_executable(luac src/luac.c)
target_link_libraries(luac PRIVATE lua)

function(compile_lua_bytecode BYTECODE_NAME)
  # argument parsing
  set(OPTION_ARGS STRIP_DEBUG)
  set(MULTI_ARGS SOURCES)
  set(SINGLE_ARGS OUTPUT)
  cmake_parse_arguments(PARSE_ARGV 1 BYTECODE
    "${OPTION_ARGS}"
    "${SINGLE_ARGS}"
    "${MULTI_ARGS}"
  )
  
  # input sanitization
  if(NOT BYTECODE_NAME)
    message(FATAL_ERROR "compile_lua_bytecode(): name must be given")
    return()
  endif()
  if(NOT BYTECODE_SOURCES)
    message(FATAL_ERROR "compile_lua_bytecode(): SOURCES must be given")
    return()
  endif()
  if(NOT BYTECODE_OUTPUT)
    set(BYTECODE_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${BYTECODE_NAME}.lua")
  endif()
  if(NOT IS_ABSOLUTE ${BYTECODE_OUTPUT})
    set(BYTECODE_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${BYTECODE_OUTPUT}")
  endif()
  
  foreach(SOURCE ${BYTECODE_SOURCES})
    if(NOT IS_ABSOLUTE ${SOURCE})
      list(APPEND BYTECODE_SANITIZED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
    else()
      list(APPEND BYTECODE_SANITIZED_SOURCES "${SOURCE}")
    endif()
  endforeach()
  
  message(STATUS "Sources: ${BYTECODE_SOURCES}")
  message(STATUS "Sanitized sources: ${BYTECODE_SANITIZED_SOURCES}")
  message(STATUS "Output: ${BYTECODE_OUTPUT}")
  
  add_custom_command(
    OUTPUT
      "${BYTECODE_OUTPUT}"
    COMMAND
      luac $<$<CONFIG:Release>:-s> -o "${BYTECODE_OUTPUT}" ${BYTECODE_SANITIZED_SOURCES}
    DEPENDS
      ${BYTECODE_SOURCES}
    COMMENT
      "Compile Lua sources of ${BYTECODE_NAME} into byte code"
  )
endfunction(compile_lua_bytecode)