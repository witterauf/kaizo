add_executable(LinkBaseUnitTest
    ConstraintTest.cc
    FreeBlockTest.cc
    SegmentEncodingTest.cc
)
target_link_libraries(LinkBaseUnitTest
    PRIVATE
        Fuse::Link
        Catch2::Main
)
add_test(NAME LinkBaseUnitTest COMMAND LinkBaseUnitTest)