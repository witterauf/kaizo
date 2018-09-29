add_executable(AssemblerFrontendUnitTest
    frontend/LexerTest.cc
    frontend/ParserTest.cc
    frontend/ExpressionParserTest.cc
    frontend/TestInstructionParser.h
    frontend/TestInstructionParser.cc
)
target_link_libraries(AssemblerFrontendUnitTest
    PRIVATE
        Fuse::Assembler
        Catch2::Main
)
add_test(NAME AssemblerFrontendUnitTest COMMAND AssemblerFrontendUnitTest)