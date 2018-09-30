add_executable(AssemblerFrontendUnitTest
    frontend/LexerTest.cc
    frontend/ParserTest.cc
    frontend/ExpressionParserTest.cc
    frontend/TestInstructionParser.h
    frontend/TestInstructionParser.cc
    frontend/TestInstruction.h
    frontend/TestInstruction.cc
    frontend/TestBlockNamer.h
    frontend/TestBlockNamer.cc
)
target_link_libraries(AssemblerFrontendUnitTest
    PRIVATE
        Fuse::Assembler
        Catch2::Main
)
add_test(NAME AssemblerFrontendUnitTest COMMAND AssemblerFrontendUnitTest)