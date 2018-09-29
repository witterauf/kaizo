#include <diagnostics/Contracts.h>
#include <diagnostics/SourceReporter.h>
#include <fuse/assembler/frontend/ExpressionParser.h>
#include <fuse/assembler/ir/BinaryOperation.h>
#include <fuse/assembler/ir/Expression.h>
#include <fuse/assembler/ir/IntegerLiteral.h>
#include <fuse/assembler/ir/UnresolvedReference.h>
#include <fuse/assembler/symbols/SymbolTable.h>

namespace fuse::assembler {

bool ExpressionParser::isStartedBy(const Token& token) const
{
    return startsFactor(token);
}

auto ExpressionParser::parse(const std::vector<Token>& tokens, size_t index)
    -> ParseResult<std::unique_ptr<Expression>>
{
    Expects(tokens.size() > 0 && tokens.back().is(TokenKind::End));
    setSource(&tokens);
    setIndex(index);
    if (auto maybeExpression = parseExpression())
    {
        return {std::move(*maybeExpression), m_index};
    }
    else
    {
        return {};
    }
}

auto ExpressionParser::parseExpression() -> std::optional<std::unique_ptr<Expression>>
{
    Expects(isStartedBy(fetch()));
    return parseLogicalExpression();
}

static auto binaryOperator(const Token& token) -> BinaryOperator
{
    switch (token.kind())
    {
    case TokenKind::Ampersand: return BinaryOperator::And;
    case TokenKind::Asterisk: return BinaryOperator::Multiply;
    case TokenKind::DoubleAngularBracketLeft: return BinaryOperator::ShiftLeft;
    case TokenKind::DoubleAngularBracketRight: return BinaryOperator::ShiftRight;
    case TokenKind::Hat: return BinaryOperator::Xor;
    case TokenKind::Minus: return BinaryOperator::Subtract;
    case TokenKind::Pipe: return BinaryOperator::Or;
    case TokenKind::Plus: return BinaryOperator::Add;
    case TokenKind::Slash: return BinaryOperator::Divide;
    case TokenKind::Tilde: return BinaryOperator::Not;
    case TokenKind::TripleAngularBracketRight: return BinaryOperator::ArithmeticShiftRight;
    }
    InvalidCase(token.kind());
}

static bool isLogicalOperator(const Token& token)
{
    switch (token.kind())
    {
    case TokenKind::Ampersand:
    case TokenKind::Hat:
    case TokenKind::Pipe:
    case TokenKind::Tilde: return true;
    default: return false;
    }
}

template <class NextParser, class Classifier>
auto ExpressionParser::parseLevel(NextParser next, Classifier classify)
    -> std::optional<std::unique_ptr<Expression>>
{
    if (auto maybeFirst = (this->*next)())
    {
        std::unique_ptr<Expression> expression = std::move(*maybeFirst);

        while (classify(fetch()))
        {
            auto const op = binaryOperator(fetch());
            consume();
            if (startsFactor(fetch()))
            {
                if (auto maybeNext = (this->*next)())
                {
                    expression = std::make_unique<BinaryOperation>(op, std::move(expression),
                                                                   std::move(*maybeNext));
                    continue;
                }
            }
            else
            {
                reportExpectedFactor();
            }
            return {};
        }
        return std::move(expression);
    }
    return {};
}

auto ExpressionParser::parseLogicalExpression() -> std::optional<std::unique_ptr<Expression>>
{
    return parseLevel(&ExpressionParser::parseShiftingExpression, &isLogicalOperator);
}

static bool isShiftingOperator(const Token& token)
{
    switch (token.kind())
    {
    case TokenKind::DoubleAngularBracketLeft:
    case TokenKind::DoubleAngularBracketRight:
    case TokenKind::TripleAngularBracketRight: return true;
    default: return false;
    }
}

auto ExpressionParser::parseShiftingExpression() -> std::optional<std::unique_ptr<Expression>>
{
    return parseLevel(&ExpressionParser::parseSimpleExpression, &isShiftingOperator);
}

static bool isAddSub(const Token& token)
{
    switch (token.kind())
    {
    case TokenKind::Plus:
    case TokenKind::Minus: return true;
    default: return false;
    }
}

auto ExpressionParser::parseSimpleExpression() -> std::optional<std::unique_ptr<Expression>>
{
    return parseLevel(&ExpressionParser::parseTerm, &isAddSub);
}

static bool isMulDiv(const Token& token)
{
    switch (token.kind())
    {
    case TokenKind::Asterisk:
    case TokenKind::Slash: return true;
    default: return false;
    }
}

auto ExpressionParser::parseTerm() -> std::optional<std::unique_ptr<Expression>>
{
    return parseLevel(&ExpressionParser::parseFactor, &isMulDiv);
}

bool ExpressionParser::startsFactor(const Token& token) const
{
    switch (token.kind())
    {
    case TokenKind::Identifier: return true;
    case TokenKind::Integer: return true;
    case TokenKind::ParenthesisLeft: return true;
    default: return false;
    }
}

auto ExpressionParser::parseFactor() -> std::optional<std::unique_ptr<Expression>>
{
    if (fetch().is(TokenKind::ParenthesisLeft))
    {
        consume();
        if (auto maybeExpression = parseExpression())
        {
            if (expectAndConsume(TokenKind::ParenthesisRight))
            {
                return std::move(maybeExpression);
            }
        }
    }
    else if (fetch().is(TokenKind::Integer))
    {
        return std::make_unique<IntegerLiteral>(fetchAndConsume().integer());
    }
    else if (fetch().is(TokenKind::Identifier))
    {
        return std::make_unique<UnresolvedReference>(fetchAndConsume().identifier());
    }
    return {};
}

//## [ diagnostics ]###############################################################################

using namespace diagnostics;

void ExpressionParser::reportExpectedFactor()
{
    if (hasReporter())
    {
        auto snippet = reporter()
                           .snippetBuilder()
                           .setSourceRange(fetch().start(), fetch().start() + 1)
                           .setMarkedRange(fetch().start(), fetch().start() + 1)
                           .setCursor(fetch().start())
                           .build();
        reporter()
            .report("expected an expression", fetch().start())
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::ExpectedFactor)
            .snippet(std::move(snippet));
    }
}

} // namespace fuse::assembler