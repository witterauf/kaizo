set(FUSE_ASSEMBLER_INCLUDE_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(FUSE_ASSEMBLER_INCLUDE_DIR "${FUSE_ASSEMBLER_INCLUDE_BASE_DIR}/fuse/assembler")

set(FUSE_ASSEMBLER_FRONTEND_SOURCES
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Classifier.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/ExpressionParser.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/InstructionParser.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Lexer.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Parser.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Token.h
    frontend/ExpressionParser.cc
    frontend/Lexer.cc
    frontend/Parser.cc
    frontend/Token.cc
)

set(FUSE_ASSEMBLER_IR_SOURCES
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/BinaryOperation.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/Expression.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/IntegerLiteral.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/Reference.h
    ir/BinaryOperation.cc
    ir/Expression.cc
    ir/IntegerLiteral.cc
    ir/Reference.cc
)

source_group("Frontend" FILES ${FUSE_ASSEMBLER_FRONTEND_SOURCES})
source_group("Intermediate" FILES ${FUSE_ASSEMBLER_IR_SOURCES})

add_library(FuseAssembler STATIC
    ${FUSE_ASSEMBLER_FRONTEND_SOURCES}
    ${FUSE_ASSEMBLER_IR_SOURCES}
)
target_link_libraries(FuseAssembler PUBLIC Diagnostics::Diagnostics)
target_include_directories(FuseAssembler
    PUBLIC
        $<BUILD_INTERFACE:${FUSE_ASSEMBLER_INCLUDE_BASE_DIR}>
)
target_all_warnings(FuseAssembler)
target_warnings_are_errors(FuseAssembler)
target_compile_features(FuseAssembler PUBLIC cxx_std_17)
add_library(Fuse::Assembler ALIAS FuseAssembler)