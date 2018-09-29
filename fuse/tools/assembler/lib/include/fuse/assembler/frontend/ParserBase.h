#pragma once

#include "ParseResult.h"
#include "Token.h"
#include <vector>

namespace diagnostics {
class SourceReporter;
}

namespace fuse::assembler {

class ParserBase
{
public:
    struct DiagnosticTags
    {
        static constexpr char UnexpectedToken[] = "ExpressionParser.UnexpectedToken";
    };

    virtual void setReporter(diagnostics::SourceReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::SourceReporter&;

    void setSource(const std::vector<Token>* tokens);
    void setIndex(size_t index);

protected:
    bool expect(TokenKind kind);
    bool expectAndConsume(TokenKind kind);
    auto fetch(size_t offset = 0) const -> const Token&;
    void consume(size_t size = 1);
    auto fetchAndConsume() -> const Token&;
    auto position() const -> size_t;

    const std::vector<Token>* m_tokens{nullptr};
    size_t m_index = 0;

    void reportUnexpectedToken();

    diagnostics::SourceReporter* m_reporter{nullptr};
};

} // namespace fuse::assembler