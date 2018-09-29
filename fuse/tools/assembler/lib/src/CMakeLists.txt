set(FUSE_ASSEMBLER_INCLUDE_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(FUSE_ASSEMBLER_INCLUDE_DIR "${FUSE_ASSEMBLER_INCLUDE_BASE_DIR}/fuse/assembler")

set(FUSE_ASSEMBLER_FRONTEND_SOURCES
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Classifier.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/ExpressionParser.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/InstructionParser.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Lexer.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Parser.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/ParserBase.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/ParseResult.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/frontend/Token.h
    frontend/ExpressionParser.cc
    frontend/Lexer.cc
    frontend/Parser.cc
    frontend/ParserBase.cc
    frontend/Token.cc
)

set(FUSE_ASSEMBLER_IR_SOURCES
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/AbstractSyntaxTree.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/AnonymousLabel.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/BinaryOperation.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/Block.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/BlockElement.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/Expression.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/ExpressionOperand.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/Instruction.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/InstructionOperand.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/IntegerLiteral.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/Label.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/NamedLabel.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/RegisterOperand.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/SymbolReference.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/ir/UnresolvedReference.h
    ir/AbstractSyntaxTree.cc
    ir/AnonymousLabel.cc
    ir/BinaryOperation.cc
    ir/Block.cc
    ir/BlockElement.cc
    ir/Expression.cc
    ir/IntegerLiteral.cc
    ir/Label.cc
    ir/NamedLabel.cc
    ir/SymbolReference.cc
    ir/UnresolvedReference.cc
)

set(FUSE_ASSEMBLER_SYMBOLS_SOURCES
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/symbols/Symbol.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/symbols/SymbolTable.h
    ${FUSE_ASSEMBLER_INCLUDE_DIR}/symbols/Type.h
    symbols/Symbol.cc
    symbols/SymbolTable.cc
    symbols/Type.cc
)

source_group("Frontend" FILES ${FUSE_ASSEMBLER_FRONTEND_SOURCES})
source_group("Intermediate" FILES ${FUSE_ASSEMBLER_IR_SOURCES})
source_group("Symbols" FILES ${FUSE_ASSEMBLER_SYMBOLS_SOURCES})

add_library(FuseAssembler STATIC
    ${FUSE_ASSEMBLER_FRONTEND_SOURCES}
    ${FUSE_ASSEMBLER_IR_SOURCES}
    ${FUSE_ASSEMBLER_SYMBOLS_SOURCES}
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