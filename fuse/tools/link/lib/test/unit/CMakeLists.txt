add_executable(LinkBaseUnitTest
    SegmentEncodingTest.cc
)
target_link_libraries(LinkBaseUnitTest
    PRIVATE
        Fuse::Link
        Catch2
)
add_test(NAME LinkBaseUnitTest COMMAND LinkBaseUnitTest)