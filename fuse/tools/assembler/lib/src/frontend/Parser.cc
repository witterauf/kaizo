#include <diagnostics/Contracts.h>
#include <functional>
#include <fuse/assembler/frontend/BlockNamer.h>
#include <fuse/assembler/frontend/InstructionParser.h>
#include <fuse/assembler/frontend/Parser.h>
#include <fuse/assembler/ir/AbstractSyntaxTree.h>
#include <fuse/assembler/ir/Annotation.h>
#include <fuse/assembler/ir/AnonymousLabel.h>
#include <fuse/assembler/ir/Block.h>
#include <fuse/assembler/ir/BlockElement.h>
#include <fuse/assembler/ir/Expression.h>
#include <fuse/assembler/ir/Instruction.h>
#include <fuse/assembler/ir/NamedLabel.h>
#include <fuse/assembler/symbols/Symbol.h>
#include <fuse/assembler/symbols/SymbolTable.h>
#include <fuse/assembler/symbols/Type.h>
#include <map>

namespace fuse::assembler {

void Parser::setReporter(diagnostics::SourceReporter* reporter)
{
    m_expressionParser.setReporter(reporter);
    ParserBase::setReporter(reporter);
}

void Parser::setSymbolTable(SymbolTable* symbolTable)
{
    Expects(symbolTable);
    m_symbolTable = symbolTable;
}

void Parser::setInstructionParser(InstructionParser* parser)
{
    Expects(parser);
    m_instructionParser = parser;
}

void Parser::setBlockNamer(BlockNamer* blockNamer)
{
    Expects(blockNamer);
    m_blockNamer = blockNamer;
}

auto Parser::parseTop() -> std::optional<std::unique_ptr<AbstractSyntaxTree>>
{
    auto ast = std::make_unique<AbstractSyntaxTree>();
    while (!fetch().is(TokenKind::End))
    {
        if (!parseTopElement(*ast))
        {
            return {};
        }
    }
    return std::move(ast);
}

bool Parser::parseTopElement(AbstractSyntaxTree& ast)
{
    switch (fetch().kind())
    {
    case TokenKind::KeywordConstant:
        if (auto maybeConstant = parseConstantDeclaration())
        {
            actOnConstantDeclaration(std::move(*maybeConstant));
            return true;
        }
        break;

    case TokenKind::KeywordExtern:
        if (auto maybeExtern = parseExternalDeclaration())
        {
            actOnExternalDeclaration(std::move(*maybeExtern));
            return true;
        }
        break;

    case TokenKind::Annotation:
        if (auto maybeBlock = parseAnnotatedBlock())
        {
            ast.append(std::move(*maybeBlock));
            return true;
        }
        break;

    case TokenKind::KeywordBlock:
        if (auto maybeBlock = parseBlock())
        {
            ast.append(std::move(*maybeBlock));
            return true;
        }
        break;

    case TokenKind::KeywordSubroutine:
        if (auto maybeSubroutine = parseSubroutine())
        {
            ast.append(std::move(*maybeSubroutine));
            return true;
        }
        break;

    default: break;
    }
    return false;
}

auto Parser::parseAnnotatedBlock() -> std::optional<std::unique_ptr<Block>>
{
    if (auto maybeAnnotations = parseAnnotationList())
    {
        if (fetch().is(TokenKind::KeywordBlock))
        {
            if (auto maybeBlock = parseBlock())
            {
                for (auto&& annotation : *maybeAnnotations)
                {
                    maybeBlock.value()->append(std::move(annotation));
                }
            }
        }
        else if (fetch().is(TokenKind::KeywordSubroutine))
        {
            if (auto maybeSubroutine = parseSubroutine())
            {
                for (auto&& annotation : *maybeAnnotations)
                {
                    maybeSubroutine.value()->append(std::move(annotation));
                }
            }
        }
        else
        {
            reportExpectedBlock();
        }
    }
    return {};
}

auto Parser::parseAnnotationList() -> std::optional<std::vector<std::unique_ptr<Annotation>>>
{
    std::vector<std::unique_ptr<Annotation>> annotations;
    while (fetch().is(TokenKind::Annotation))
    {
        if (auto maybeAnnotation = parseAnnotation())
        {
            annotations.push_back(std::move(*maybeAnnotation));
        }
        else
        {
            return {};
        }
    }
    return std::move(annotations);
}

auto Parser::parseAnnotation() -> std::optional<std::unique_ptr<Annotation>>
{
    if (!expect(TokenKind::Annotation))
    {
        return {};
    }
    auto annotation = std::make_unique<Annotation>(fetchAndConsume().annotation());
    if (fetch().is(TokenKind::ParenthesisLeft))
    {
        consume();
        if (!parseAnnotationArguments(*annotation))
        {
            return {};
        }
        if (!expectAndConsume(TokenKind::ParenthesisRight))
        {
            return {};
        }
    }
    return std::move(annotation);
}

bool Parser::parseAnnotationArguments(Annotation& annotation)
{
    for (;;)
    {
        if (fetch().is(TokenKind::Identifier))
        {
            auto const identifier = fetchAndConsume().identifier();
            if (!expectAndConsume(TokenKind::Equal))
            {
                return false;
            }
            if (!expect(TokenKind::Integer))
            {
                return false;
            }
            annotation.addNamed(identifier, fetchAndConsume().integer());
        }
        else if (fetch().is(TokenKind::Integer))
        {
            annotation.addPositional(fetchAndConsume().integer());
        }

        if (!fetch().is(TokenKind::Comma))
        {
            break;
        }
        consume();
    };
    return true;
}

auto Parser::parseSubroutine() -> std::optional<std::unique_ptr<Block>>
{
    if (!expectAndConsume(TokenKind::KeywordSubroutine))
    {
        return {};
    }
    if (!expect(TokenKind::Identifier))
    {
        return {};
    }

    auto const identifier = fetchAndConsume().identifier();
    auto block = std::make_unique<Block>(identifier);
    auto labelSymbol = Symbol::makeLabel(identifier);
    auto label = std::make_unique<NamedLabel>(labelSymbol.get());
    block->append(std::move(label));
    actOnLabel(std::move(labelSymbol));

    bool blockSuccess = parseBlockBody(*block);
    if (!blockSuccess)
    {
        return {};
    }
    return std::move(block);
}

auto Parser::parseBlock() -> std::optional<std::unique_ptr<Block>>
{
    Expects(m_blockNamer);
    if (!expectAndConsume(TokenKind::KeywordBlock))
    {
        return {};
    }
    auto block = std::make_unique<Block>(m_blockNamer->generateName());
    if (parseBlockBody(*block))
    {
        return std::move(block);
    }
    else
    {
        return {};
    }
}

bool Parser::parseBlockBody(Block& block)
{
    if (!expectAndConsume(TokenKind::CurlyBraceLeft))
    {
        return {};
    }

    bool blockSuccess{true};
    while (!fetch().is(TokenKind::CurlyBraceRight))
    {
        if (auto maybeElement = parseBlockElement())
        {
            block.append(std::move(*maybeElement));
        }
        else
        {
            blockSuccess = false;
        }
    }
    consume(); // '}'
    return blockSuccess;
}

auto Parser::parseBlockElement() -> std::optional<std::unique_ptr<BlockElement>>
{
    switch (fetch().kind())
    {
    case TokenKind::Plus:
        if (auto maybeAnonymousLabel = parseAnonymousForwardLabel())
        {
            return std::move(*maybeAnonymousLabel);
        }
        break;

    case TokenKind::Minus:
        if (auto maybeAnonymousLabel = parseAnonymousBackwardLabel())
        {
            return std::move(*maybeAnonymousLabel);
        }
        break;

    case TokenKind::Identifier:
        if (auto maybeLabel = parseLabel())
        {
            return std::move(*maybeLabel);
        }
        break;

    case TokenKind::Mnemonic:
        if (auto maybeInstruction = parseInstruction())
        {
            return std::move(*maybeInstruction);
        }
        break;

    case TokenKind::Directive: return {};
    default: break;
    }
    return {};
}

auto Parser::parseLabel() -> std::optional<std::unique_ptr<NamedLabel>>
{
    if (!expect(TokenKind::Identifier))
    {
        return {};
    }
    auto const identifier = fetchAndConsume().identifier();
    if (!expectAndConsume(TokenKind::Colon))
    {
        return {};
    }

    auto labelSymbol = Symbol::makeLabel(identifier);
    auto label = std::make_unique<NamedLabel>(labelSymbol.get());
    actOnLabel(std::move(labelSymbol));
    return std::move(label);
}

auto Parser::parseAnonymousForwardLabel() -> std::optional<std::unique_ptr<AnonymousLabel>>
{
    if (!expectAndConsume(TokenKind::Plus))
    {
        return {};
    }
    size_t level{1};
    while (fetch().is(TokenKind::Plus))
    {
        consume();
        level += 1;
    }
    return AnonymousLabel::makeForward(level);
}

auto Parser::parseAnonymousBackwardLabel() -> std::optional<std::unique_ptr<AnonymousLabel>>
{
    if (!expectAndConsume(TokenKind::Minus))
    {
        return {};
    }
    size_t level{1};
    while (fetch().is(TokenKind::Minus))
    {
        consume();
        level += 1;
    }
    return AnonymousLabel::makeBackward(level);
}

auto Parser::parseConstantDeclaration() -> std::optional<std::unique_ptr<Symbol>>
{
    if (!expectAndConsume(TokenKind::KeywordConstant))
        return {};
    if (!expect(TokenKind::Identifier))
        return {};
    auto const identifier = fetchAndConsume().identifier();
    if (!expectAndConsume(TokenKind::Colon))
    {
        return {};
    }
    auto maybeType = parseDeclarationType();
    if (!maybeType)
    {
        return {};
    }
    if (!expectAndConsume(TokenKind::Equal))
    {
        return {};
    }
    auto maybeExpression = parseExpression();
    if (!maybeExpression)
    {
        return {};
    }

    return Symbol::makeConstant(SymbolIdentifier{identifier}, std::move(*maybeType),
                                std::move(*maybeExpression));
}

static const std::map<std::string, std::function<std::unique_ptr<Type>()>> BuiltInTypes = {
    {"byte", []() { return Type::makeUnsigned(8); }},
    {"word", []() { return Type::makeUnsigned(16); }},
    {"unsigned24", []() { return Type::makeUnsigned(24); }},
    {"dword", []() { return Type::makeUnsigned(32); }}};

auto Parser::parseDeclarationType() -> std::optional<std::unique_ptr<Type>>
{
    if (!expect(TokenKind::Identifier))
    {
        return {};
    }
    auto const builder = BuiltInTypes.find(fetchAndConsume().identifier());
    if (builder != BuiltInTypes.cend())
    {
        return builder->second();
    }
    else
    {
        reportUnknownType();
        return {};
    }
}

auto Parser::parseExternalDeclaration() -> std::optional<std::unique_ptr<Symbol>>
{
    if (!expectAndConsume(TokenKind::KeywordExtern))
    {
        return {};
    }
    if (!expect(TokenKind::Identifier))
    {
        return {};
    }
    auto const identifier = fetchAndConsume().identifier();
    return Symbol::makeExternal(identifier);
}

auto Parser::parseExpression() -> std::optional<std::unique_ptr<Expression>>
{
    if (auto result = m_expressionParser.parse(*m_tokens, m_index))
    {
        m_index = result.index();
        return std::move(*result);
    }
    else
    {
        return {};
    }
}

auto Parser::parseInstruction() -> std::optional<std::unique_ptr<Instruction>>
{
    Expects(m_instructionParser);
    if (auto result = m_instructionParser->parse(*m_tokens, m_index))
    {
        m_index = result.index();
        return std::move(*result);
    }
    else
    {
        return {};
    }
}

void Parser::actOnConstantDeclaration(std::unique_ptr<Symbol>&& constant)
{
    Expects(m_symbolTable);
    Expects(constant->isConstant());
    m_symbolTable->add(std::move(constant));
}

void Parser::actOnExternalDeclaration(std::unique_ptr<Symbol>&& external)
{
    Expects(m_symbolTable);
    Expects(external->isExternal());
    m_symbolTable->add(std::move(external));
}

void Parser::actOnLabel(std::unique_ptr<Symbol>&& label)
{
    Expects(m_symbolTable);
    Expects(label->isLabel());
    m_symbolTable->add(std::move(label));
}

//##[ diagnostics ]################################################################################

void Parser::reportUnknownType()
{
}

void Parser::reportDuplicateDeclaration()
{
}

void Parser::reportExpectedBlock()
{
}

} // namespace fuse::assembler