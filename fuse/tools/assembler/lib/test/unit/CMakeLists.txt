add_executable(AssemblerFrontendUnitTest
    LexerTest.cc
    ParserTest.cc
)
target_link_libraries(AssemblerFrontendUnitTest
    PRIVATE
        Fuse::Assembler
        Catch2::Main
)
add_test(NAME AssemblerFrontendUnitTest COMMAND AssemblerFrontendUnitTest)