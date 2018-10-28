add_executable(GraphicsUnitTest
    graphics/TilesTest.cc
    graphics/LuaTest.cc
    graphics/LuaFontLoaderTest.cc
)
target_link_libraries(GraphicsUnitTest
    PRIVATE
        Fuse::Fuse
        Catch2::Main
        sol2
)

add_executable(TextUnitTest
    text/TableTest.cc
)
target_link_libraries(TextUnitTest
    PRIVATE
        Fuse::Fuse
)

add_executable(BinaryUnitTest
    binary/DataFormatTest.cc
)
target_link_libraries(BinaryUnitTest
    PRIVATE
        Fuse::Fuse
)