#include <cctype>
#include <diagnostics/Contracts.h>
#include <diagnostics/SourceReporter.h>
#include <fstream>
#include <fuse/assembler/frontend/Classifier.h>
#include <fuse/assembler/frontend/Lexer.h>

namespace fs = std::filesystem;

namespace fuse::assembler {

void Lexer::setReporter(diagnostics::SourceReporter* reporter)
{
    m_reporter = reporter;
}

bool Lexer::hasReporter() const
{
    return m_reporter != nullptr;
}

auto Lexer::reporter() -> diagnostics::SourceReporter*
{
    return m_reporter;
}

void Lexer::setClassifier(const Classifier* classifier)
{
    m_classifier = classifier;
}

auto Lexer::lex(const char* source, size_t size) -> std::optional<std::vector<Token>>
{
    Expects(source);
    Expects(size > 0);
    m_source = source;
    m_sourceSize = size;
    return lex();
}

auto Lexer::lex() -> std::optional<std::vector<Token>>
{
    std::vector<Token> tokens;
    while (hasNext())
    {
        lexWhitespace();
        if (auto maybeToken = lexToken())
        {
            if (isEndOfInstruction(*maybeToken))
            {
                endInstruction();
            }
            if (wasEndOfInstruction())
            {
                auto endOfInstruction = Token::makeEndOfInstruction();
                endOfInstruction.setRange(maybeToken->start(), maybeToken->start());
                tokens.push_back(std::move(endOfInstruction));
                m_instructionEnded = false;
            }
            tokens.push_back(std::move(*maybeToken));
        }
    }
    if (isInstructionActive())
    {
        auto endOfInstruction = Token::makeEndOfInstruction();
        endOfInstruction.setRange(position(), position());
        tokens.push_back(std::move(endOfInstruction));
    }
    tokens.push_back(Token::makeEnd());
    return tokens;
}

void Lexer::lexWhitespace()
{
    while (true)
    {
        if (std::isspace(fetch()))
        {
            if (isInstructionActive() && (fetch() == '\n' || fetch() == '\r'))
            {
                endInstruction();
            }
            continue;
        }
        if (fetch() == ';')
        {
            do
            {
                consume();
            } while (fetch() != '\n' && fetch() != '\r');
            continue;
        }
        return;
    }
}

auto Lexer::lexToken() -> std::optional<Token>
{
    std::optional<Token> token;
    m_tokenStart = position();
    switch (fetch())
    {
    case ':':
        consume();
        token = Token::makeSymbol(TokenKind::Colon);
        break;
    case '#':
        consume();
        token = Token::makeSymbol(TokenKind::PoundSign);
        break;
    case '(':
        consume();
        token = Token::makeSymbol(TokenKind::ParenthesisLeft);
        break;
    case ')':
        consume();
        token = Token::makeSymbol(TokenKind::ParenthesisRight);
        break;
    case '[':
        consume();
        token = Token::makeSymbol(TokenKind::SquareBracketLeft);
        break;
    case ']':
        consume();
        token = Token::makeSymbol(TokenKind::SquareBracketRight);
        break;
    case '{':
        consume();
        m_braceLevel += 1;
        token = Token::makeSymbol(TokenKind::CurlyBraceLeft);
        break;
    case '}':
        consume();
        m_braceLevel -= 1;
        token = Token::makeSymbol(TokenKind::CurlyBraceRight);
        break;
    case '+':
        consume();
        token = Token::makeSymbol(TokenKind::Plus);
        break;
    case '-':
        consume();
        token = Token::makeSymbol(TokenKind::Minus);
        break;
    case '/':
        consume();
        token = Token::makeSymbol(TokenKind::Slash);
        break;
    case '*':
        consume();
        token = Token::makeSymbol(TokenKind::Asterisk);
        break;
    case '=':
        consume();
        token = Token::makeSymbol(TokenKind::Equal);
        break;
    case '\\':
        consume();
        token = Token::makeSymbol(TokenKind::Backslash);
        break;
    case ',':
        consume();
        token = Token::makeSymbol(TokenKind::Comma);
        break;
    case '\'':
        consume();
        token = Token::makeSymbol(TokenKind::SingleQuote);
        break;
    case '"':
        consume();
        token = Token::makeSymbol(TokenKind::DoubleQuote);
        break;

    case '@': token = lexAnnotation(); break;
    case '.': token = lexDirective(); break;

    case '$':
        consume();
        if (std::isxdigit(fetch()))
        {
            token = lexHexadecimal();
        }
        else
        {
            token = Token::makeSymbol(TokenKind::Dollar);
        }
        break;

    case '%':
        consume();
        if (fetch() == '0' || fetch() == '1')
        {
            token = lexBinary();
        }
        else
        {
            token = Token::makeSymbol(TokenKind::Percent);
        }
        break;
    default:
        if (std::isalpha(fetch()))
        {
            token = lexIdentifier();
        }
        else if (std::isdigit(fetch()))
        {
            token = lexDecimal();
        }
        else
        {
            reportUnexpectedCharacter();
        }
    }

    if (token.has_value())
    {
        auto const end = position();
        token->setRange(m_tokenStart, end);
        return token;
    }
    else
    {
        return {};
    }
}

static bool continuesIdentifier(char c)
{
    return std::isalnum(c) || c == '_' || c == '.';
}

auto Lexer::lexIdentifier() -> std::optional<Token>
{
    auto const identifier = lexIdentifierString();
    if (m_classifier)
    {
        auto const classification = m_classifier->classify(identifier);
        switch (classification.kind)
        {
        case IdentifierKind::Identifier: return Token::makeIdentifier(identifier);
        case IdentifierKind::Mnemonic:
            startInstruction();
            return Token::makeMnemonic(*classification.id);
        case IdentifierKind::Keyword: return Token::makeCustomKeyword(*classification.id);
        case IdentifierKind::Register: return Token::makeRegister(*classification.id);
        }
        InvalidCase(classification.kind);
    }
    else
    {
        return Token::makeIdentifier(identifier);
    }
}

void Lexer::startInstruction()
{
    if (m_instructionActive)
    {
        m_instructionEnded = true;
    }
    m_instructionActive = true;
    m_braceLevel = 0;
}

bool Lexer::isEndOfInstruction(const Token& token) const
{
    return token.is(TokenKind::Backslash) ||
           (m_braceLevel == 0 && token.is(TokenKind::CurlyBraceRight));
}

void Lexer::endInstruction()
{
    m_instructionActive = false;
    m_instructionEnded = true;
    m_braceLevel = 0;
}

bool Lexer::isInstructionActive() const
{
    return m_instructionActive;
}

bool Lexer::wasEndOfInstruction() const
{
    return m_instructionEnded;
}

auto Lexer::lexIdentifierString() -> std::string
{
    std::string identifier;
    do
    {
        identifier += fetchAndConsume();
    } while (hasNext() && continuesIdentifier(fetch()));
    return identifier;
}

auto Lexer::lexAnnotation() -> std::optional<Token>
{
    consume();
    if (std::isalpha(fetch()))
    {
        auto const annotation = lexIdentifierString();
        return Token::makeAnnotation(annotation);
    }
    else
    {
        return Token::makeSymbol(TokenKind::At);
    }
}

auto Lexer::lexDirective() -> std::optional<Token>
{
    consume();
    if (std::isalpha(fetch()))
    {
        auto const directive = lexIdentifierString();
        return Token::makeDirective(directive);
    }
    else
    {
        return Token::makeSymbol(TokenKind::Dot);
    }
}

auto Lexer::lexDecimal() -> std::optional<Token>
{
    bool success{true};
    int64_t value = 0;
    do
    {
        if (value * 10 < value)
        {
            reportIntegerDoesNotFit();
            success = false;
        }

        value *= 10;
        value += static_cast<int64_t>(fetchAndConsume() - '0');
    } while (hasNext() && std::isdigit(fetch()));

    if (success)
    {
        return Token::makeInteger(value);
    }
    else
    {
        return {};
    }
}

auto Lexer::lexBinary() -> std::optional<Token>
{
    bool success{true};
    int64_t value = 0;
    do
    {
        if (value * 2 < value)
        {
            reportIntegerDoesNotFit();
            success = false;
        }

        value *= 2;
        value += static_cast<int64_t>(fetchAndConsume() - '0');
    } while (hasNext() && (fetch() == '0' || fetch() == '1'));

    if (success)
    {
        return Token::makeInteger(value);
    }
    else
    {
        return {};
    }
}

static auto hexValue(char c) -> int64_t
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<int64_t>(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return static_cast<int64_t>(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
        return static_cast<int64_t>(c - 'A' + 10);
    }
    else
    {
        throw std::logic_error{"should not happen"};
    }
}

auto Lexer::lexHexadecimal() -> std::optional<Token>
{
    bool success{true};
    int64_t value = 0;
    do
    {
        if (value * 16 < value)
        {
            reportIntegerDoesNotFit();
            success = false;
        }

        value *= 16;
        value += hexValue(fetchAndConsume());
    } while (hasNext() && std::isxdigit(fetch()));

    if (success)
    {
        return Token::makeInteger(value);
    }
    else
    {
        return {};
    }
}

bool Lexer::hasNext() const
{
    return m_offset < m_sourceSize;
}

void Lexer::consume(size_t size)
{
    m_offset += size;
}

auto Lexer::fetch(size_t offset) const -> char
{
    if (m_offset + offset < m_sourceSize)
    {
        return m_source[m_offset + offset];
    }
    else
    {
        return '\0';
    }
}

auto Lexer::fetchAndConsume() -> char
{
    auto const c = fetch();
    consume();
    return c;
}

auto Lexer::position() const -> size_t
{
    return m_offset;
}

auto Lexer::tokenStart() const -> size_t
{
    return m_tokenStart;
}

//##[ diagnostics ]################################################################################

using namespace diagnostics;

void Lexer::reportUnexpectedCharacter()
{
    if (hasReporter())
    {
        auto snippet = reporter()
                           ->snippetBuilder()
                           .setCursor(tokenStart())
                           .setSourceRange(tokenStart(), tokenStart() + 1)
                           .setMarkedRange(tokenStart(), tokenStart() + 1)
                           .build();
        reporter()
            ->report("unexpected character", tokenStart())
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::UnexpectedCharacter)
            .snippet(std::move(snippet));
    }
}

void Lexer::reportIntegerDoesNotFit()
{
    if (hasReporter())
    {
        auto snippet = reporter()
                           ->snippetBuilder()
                           .setCursor(tokenStart())
                           .setSourceRange(tokenStart(), tokenStart() + 1)
                           .setMarkedRange(tokenStart(), tokenStart() + 1)
                           .build();
        reporter()
            ->report("integer does not fit (maximum: signed 64 bits)", tokenStart())
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::IntegerDoesNotFit)
            .snippet(std::move(snippet));
    }
}

} // namespace fuse::assembler