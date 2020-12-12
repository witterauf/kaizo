add_library(LodePNG
    lodepng.h
    lodepng.cpp
)
target_include_directories(LodePNG
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
add_library(LodePNG::LodePNG ALIAS LodePNG)