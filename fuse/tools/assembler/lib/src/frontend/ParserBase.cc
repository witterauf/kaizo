#include <diagnostics/Contracts.h>
#include <fuse/assembler/frontend/ParserBase.h>
#include <diagnostics/SourceReporter.h>

namespace fuse::assembler {

void ParserBase::setReporter(diagnostics::SourceReporter* reporter)
{
    m_reporter = reporter;
}

bool ParserBase::hasReporter() const
{
    return m_reporter != nullptr;
}

auto ParserBase::reporter() -> diagnostics::SourceReporter&
{
    Expects(hasReporter());
    return *m_reporter;
}

void ParserBase::setSource(const std::vector<Token>* tokens)
{
    Expects(tokens);
    m_tokens = tokens;
}

void ParserBase::setIndex(size_t index)
{
    Expects(index < m_tokens->size());
    m_index = index;
}

bool ParserBase::expect(TokenKind kind)
{
    if (!fetch().is(kind))
    {
        reportUnexpectedToken();
        return false;
    }
    return true;
}

bool ParserBase::expectAndConsume(TokenKind kind)
{
    if (fetch().is(kind))
    {
        consume();
        return true;
    }
    else
    {
        reportUnexpectedToken();
        return false;
    }
}

auto ParserBase::fetch(size_t offset) const -> const Token&
{
    Expects(m_tokens);
    if (m_index + offset < m_tokens->size())
    {
        return (*m_tokens)[m_index + offset];
    }
    else
    {
        return m_tokens->back();
    }
}

void ParserBase::consume(size_t size)
{
    m_index += size;
}

auto ParserBase::fetchAndConsume() -> const Token&
{
    auto const& token = fetch();
    consume();
    return token;
}

auto ParserBase::position() const -> size_t
{
    return m_index;
}

//##[ diagnostics ]################################################################################

using namespace diagnostics;

void ParserBase::reportUnexpectedToken()
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
            .tag(DiagnosticTags::UnexpectedToken)
            .snippet(std::move(snippet));
    }
}

} // namespace fuse::assembler