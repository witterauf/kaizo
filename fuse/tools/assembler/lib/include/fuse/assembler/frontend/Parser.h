#pragma once

#include "ExpressionParser.h"
#include "ParserBase.h"
#include "Token.h"
#include <optional>
#include <vector>

namespace fuse::assembler {

class InstructionParser;
class Instruction;
class Symbol;
class SymbolTable;
class Type;
class Subroutine;
class Block;
class NamedLabel;
class AnonymousLabel;
class AbstractSyntaxTree;

class Parser : public ParserBase
{
public:
    void setReporter(diagnostics::SourceReporter* reporter) override;
    void setInstructionParser(InstructionParser* parser);
    void setSymbolTable(SymbolTable* symbolTable);

    auto parseTop() -> std::optional<std::unique_ptr<AbstractSyntaxTree>>;

    auto parseBlock() -> std::optional<std::unique_ptr<Block>>;
    auto parseSubroutine() -> std::optional<std::unique_ptr<Block>>;
    bool parseBlockBody(Block& block);

    auto parseLabel() -> std::optional<std::unique_ptr<NamedLabel>>;
    auto parseAnonymousForwardLabel() -> std::optional<std::unique_ptr<AnonymousLabel>>;
    auto parseAnonymousBackwardLabel() -> std::optional<std::unique_ptr<AnonymousLabel>>;
    auto parseAnnotation();
    auto parseDirective();

    auto parseConstantDeclaration() -> std::optional<std::unique_ptr<Symbol>>;
    auto parseDeclarationType() -> std::optional<std::unique_ptr<Type>>;
    auto parseExternalDeclaration() -> std::optional<std::unique_ptr<Symbol>>;

private:
    void actOnConstantDeclaration(std::unique_ptr<Symbol>&& constant);
    void actOnExternalDeclaration(std::unique_ptr<Symbol>&& external);
    void actOnLabel(std::unique_ptr<Symbol>&& label);

    SymbolTable* m_symbolTable{nullptr};

    auto parseExpression() -> std::optional<std::unique_ptr<Expression>>;
    auto parseInstruction() -> std::optional<std::unique_ptr<Instruction>>;

    InstructionParser* m_instructionParser{nullptr};
    ExpressionParser m_expressionParser;

    void reportUnknownType();
    void reportDuplicateDeclaration();

    diagnostics::SourceReporter* m_reporter{nullptr};
};

} // namespace fuse::assembler