add_executable(GraphicsUnitTest
    graphics/TilesTest.cc
)
target_link_libraries(GraphicsUnitTest
    PRIVATE
        Fuse::Fuse
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