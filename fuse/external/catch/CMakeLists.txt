add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Catch2Main STATIC
    Catch2.hpp
    CatchMain.cc
)
target_link_libraries(Catch2Main PUBLIC Catch2)

add_library(Catch2::Catch2 ALIAS Catch2)
add_library(Catch2::Main ALIAS Catch2Main)