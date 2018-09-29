#include <diagnostics/Contracts.h>
#include <fuse/assembler/frontend/Token.h>

namespace fuse::assembler {

auto Token::makeEnd() -> Token
{
    Token token;
    token.m_kind = TokenKind::End;
    return token;
}

auto Token::makeEndOfInstruction() -> Token
{
    Token token;
    token.m_kind = TokenKind::EndOfInstruction;
    return token;
}

auto Token::makeIdentifier(const std::string& identifier) -> Token
{
    Token token;
    token.m_kind = TokenKind::Identifier;
    token.m_content = identifier;
    return token;
}

auto Token::makeAnnotation(const std::string& name) -> Token
{
    Token token;
    token.m_kind = TokenKind::Annotation;
    token.m_content = name;
    return token;
}

auto Token::makeDirective(const std::string& name) -> Token
{
    Token token;
    token.m_kind = TokenKind::Directive;
    token.m_content = name;
    return token;
}

auto Token::makeInteger(int64_t value) -> Token
{
    Token token;
    token.m_kind = TokenKind::Integer;
    token.m_content = value;
    return token;
}

auto Token::makeSymbol(TokenKind symbol) -> Token
{
    Token token;
    token.m_kind = symbol;
    return token;
}

auto Token::makeMnemonic(size_t id) -> Token
{
    Token token;
    token.m_kind = TokenKind::Mnemonic;
    token.m_content = id;
    return token;
}

auto Token::makeCustomKeyword(size_t id) -> Token
{
    Token token;
    token.m_kind = TokenKind::CustomKeyword;
    token.m_content = id;
    return token;
}

auto Token::makeRegister(size_t id) -> Token
{
    Token token;
    token.m_kind = TokenKind::Register;
    token.m_content = id;
    return token;
}

void Token::setRange(size_t start, size_t end)
{
    m_start = start;
    m_end = end;
}

bool Token::is(TokenKind kind) const
{
    return m_kind == kind;
}

auto Token::start() const -> size_t
{
    return m_start;
}

auto Token::end() const -> size_t
{
    return m_end;
}

auto Token::kind() const -> TokenKind
{
    return m_kind;
}

bool Token::isEquivalent(const Token& rhs) const
{
    return m_kind == rhs.m_kind && m_content == rhs.m_content;
}

auto Token::registerId() const -> size_t
{
    Expects(is(TokenKind::Register));
    Expects(std::holds_alternative<size_t>(m_content));
    return std::get<size_t>(m_content);
}

auto Token::mnemonicId() const -> size_t
{
    Expects(is(TokenKind::Mnemonic));
    Expects(std::holds_alternative<size_t>(m_content));
    return std::get<size_t>(m_content);
}

auto Token::customKeywordId() const -> size_t
{
    Expects(is(TokenKind::CustomKeyword));
    Expects(std::holds_alternative<size_t>(m_content));
    return std::get<size_t>(m_content);
}

auto Token::integer() const -> int64_t
{
    Expects(is(TokenKind::Integer));
    Expects(std::holds_alternative<int64_t>(m_content));
    return std::get<int64_t>(m_content);
}

} // namespace fuse::assembler