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
class BlockNamer;
class BlockElement;
class Annotation;
class Directive;

class Parser : public ParserBase
{
public:
    struct DiagnosticTags
    {
        static constexpr char UnknownType[] = "Parser.UnknownType";
        static constexpr char DuplicateDeclaration[] = "Parser.DuplicateDeclaration";
        static constexpr char ExpectedBlock[] = "Parser.ExpectedBlock";
    };

    void setReporter(diagnostics::SourceReporter* reporter) override;
    void setInstructionParser(InstructionParser* parser);
    void setSymbolTable(SymbolTable* symbolTable);
    void setBlockNamer(BlockNamer* blockNamer);

    auto parseTop() -> std::optional<std::unique_ptr<AbstractSyntaxTree>>;
    bool parseTopElement(AbstractSyntaxTree& ast);

    auto parseAnnotatedBlock() -> std::optional<std::unique_ptr<Block>>;
    auto parseBlock() -> std::optional<std::unique_ptr<Block>>;
    auto parseSubroutine() -> std::optional<std::unique_ptr<Block>>;
    bool parseBlockBody(Block& block);
    auto parseBlockElement() -> std::optional<std::unique_ptr<BlockElement>>;

    auto parseLabel() -> std::optional<std::unique_ptr<NamedLabel>>;
    auto parseAnonymousForwardLabel() -> std::optional<std::unique_ptr<AnonymousLabel>>;
    auto parseAnonymousBackwardLabel() -> std::optional<std::unique_ptr<AnonymousLabel>>;
    auto parseAnnotationList() -> std::optional<std::vector<std::unique_ptr<Annotation>>>;
    auto parseAnnotation() -> std::optional<std::unique_ptr<Annotation>>;
    bool parseAnnotationArguments(Annotation& annotation);
    auto parseDirective() -> std::optional<std::unique_ptr<Directive>>;
    bool parseDirectiveArguments(Directive& directive);

    auto parseConstantDeclaration() -> std::optional<std::unique_ptr<Symbol>>;
    auto parseDeclarationType() -> std::optional<std::unique_ptr<Type>>;
    auto parseExternalDeclaration() -> std::optional<std::unique_ptr<Symbol>>;

private:
    void actOnConstantDeclaration(std::unique_ptr<Symbol>&& constant);
    void actOnExternalDeclaration(std::unique_ptr<Symbol>&& external);
    void actOnLabel(std::unique_ptr<Symbol>&& label);

    SymbolTable* m_symbolTable{nullptr};
    BlockNamer* m_blockNamer{nullptr};

    auto parseExpression() -> std::optional<std::unique_ptr<Expression>>;
    auto parseInstruction() -> std::optional<std::unique_ptr<Instruction>>;

    InstructionParser* m_instructionParser{nullptr};
    ExpressionParser m_expressionParser;

    void reportUnknownType();
    void reportDuplicateDeclaration();
    void reportExpectedBlock();

    diagnostics::SourceReporter* m_reporter{nullptr};
};

} // namespace fuse::assembler