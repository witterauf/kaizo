#pragma once

#include "Token.h"
#include <optional>
#include <vector>

namespace diagnostics {
class SourceReporter;
}

namespace fuse::assembler {

class Expression;
class InstructionParser;

class ExpressionParser
{
public:
    void setReporter(diagnostics::SourceReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::SourceReporter*;

    bool isStartedBy(const Token& token) const;

    auto parse(const std::vector<Token>& tokens, size_t index)
        -> std::optional<std::unique_ptr<Expression>>;

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

    void setSource(const std::vector<Token>* tokens);
    void setIndex(size_t index);
    bool expectAndConsume(TokenKind kind);
    auto fetch(size_t offset = 0) const -> const Token&;
    void consume(size_t size = 1);
    auto fetchAndConsume() -> const Token&;
    auto position() const -> size_t;

    const std::vector<Token>* m_tokens = nullptr;
    size_t m_index = 0;

    void reportExpectedFactor();
    void reportUnexpectedToken();
};

} // namespace fuse::assembler