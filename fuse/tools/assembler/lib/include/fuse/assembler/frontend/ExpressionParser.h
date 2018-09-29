#pragma once

#include "ParserBase.h"
#include <optional>
#include <vector>

namespace fuse::assembler {

class Expression;
class InstructionParser;
class SymbolTable;

class ExpressionParser : public ParserBase
{
public:
    struct DiagnosticTags
    {
        static constexpr char ExpectedFactor[] = "ExpressionParser.ExpectedFactor";
    };

    bool isStartedBy(const Token& token) const;

    auto parse(const std::vector<Token>& tokens, size_t index = 0)
        -> ParseResult<std::unique_ptr<Expression>>;

    auto parseExpression() -> std::optional<std::unique_ptr<Expression>>;
    auto parseLogicalExpression() -> std::optional<std::unique_ptr<Expression>>;
    auto parseShiftingExpression() -> std::optional<std::unique_ptr<Expression>>;
    auto parseSimpleExpression() -> std::optional<std::unique_ptr<Expression>>;
    auto parseTerm() -> std::optional<std::unique_ptr<Expression>>;
    auto parseFactor() -> std::optional<std::unique_ptr<Expression>>;

private:
    template <class NextParser, class Classifier>
    auto parseLevel(NextParser next, Classifier classify)
        -> std::optional<std::unique_ptr<Expression>>;

    bool startsFactor(const Token& token) const;

    void reportExpectedFactor();

};

} // namespace fuse::assembler