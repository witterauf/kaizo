#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

namespace fuse::assembler {

enum class TokenKind
{
    // identifier-based
    Identifier,
    Register,
    Mnemonic,
    Annotation,
    Directive,
    
    // keywords
    KeywordBlock,
    KeywordConstant,
    KeywordExtern,
    KeywordSubroutine,
    CustomKeyword,

    // other literals
    Integer,

    // symbols
    Colon,
    ParenthesisLeft,
    ParenthesisRight,
    SquareBracketLeft,
    SquareBracketRight,
    CurlyBraceLeft,
    CurlyBraceRight,
    Dollar,
    PoundSign,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Backslash,
    Dot,
    Comma,
    Percent,
    Equal,
    SingleQuote,
    DoubleQuote,
    At,
    
    // logical operators
    Ampersand,
    Pipe,
    Hat,
    Tilde,

    // shifting operators
    DoubleAngularBracketLeft,
    DoubleAngularBracketRight,
    TripleAngularBracketRight,

    // meta
    EndOfInstruction,
    End
};

class Token
{
public:
    static auto makeEnd() -> Token;
    static auto makeEndOfInstruction() -> Token;
    static auto makeKeyword(TokenKind kind) -> Token;
    static auto makeIdentifier(const std::string& identifier) -> Token;
    static auto makeAnnotation(const std::string& name) -> Token;
    static auto makeDirective(const std::string& name) -> Token;
    static auto makeInteger(int64_t value) -> Token;
    static auto makeSymbol(TokenKind symbol) -> Token;
    static auto makeMnemonic(size_t id) -> Token;
    static auto makeCustomKeyword(size_t id) -> Token;
    static auto makeRegister(size_t id) -> Token;

    void setRange(size_t start, size_t end);
    auto start() const -> size_t;
    auto end() const -> size_t;

    auto kind() const -> TokenKind;
    bool is(TokenKind kind) const;

    auto registerId() const -> size_t;
    auto mnemonicId() const -> size_t;
    auto customKeywordId() const -> size_t;
    auto integer() const -> int64_t;
    auto identifier() const -> const std::string&;

    bool isEquivalent(const Token& rhs) const;

private:
    TokenKind m_kind;
    std::variant<std::string, int64_t, size_t> m_content;
    size_t m_start = 0;
    size_t m_end = 0;
};

} // namespace fuse::assembler