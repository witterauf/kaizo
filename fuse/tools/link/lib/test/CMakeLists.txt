add_library(Catch2 STATIC
    CatchMain.cc
)
target_include_directories(Catch2
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(unit)