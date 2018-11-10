add_library(lfs
    lfs.c
    lfs.h)
target_include_directories(lfs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lfs PUBLIC lua)
add_library(Lua::FileSystem ALIAS lfs)