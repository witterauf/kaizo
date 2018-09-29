#pragma once

#include "Token.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace diagnostics {
class SourceReporter;
}

namespace fuse::assembler {

class Classifier;

class Lexer
{
public:
    struct DiagnosticTags
    {
        static constexpr char UnexpectedCharacter[] = "Lexer.UnexpectedCharacter";
        static constexpr char IntegerDoesNotFit[] = "Lexer.IntegerDoesNotFit";
    };

    void setReporter(diagnostics::SourceReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::SourceReporter*;

    void setClassifier(const Classifier* classifier);
    auto lex(const char* source, size_t size) -> std::optional<std::vector<Token>>;

private:
    auto lex() -> std::optional<std::vector<Token>>;
    auto lexToken() -> std::optional<Token>;
    auto lexIdentifier() -> std::optional<Token>;
    auto lexAnnotation() -> std::optional<Token>;
    auto lexDirective() -> std::optional<Token>;
    auto lexDecimal() -> std::optional<Token>;
    auto lexBinary() -> std::optional<Token>;
    auto lexHexadecimal() -> std::optional<Token>;
    auto lexIdentifierString() -> std::string;

    void lexWhitespace();

    void startInstruction();
    void endInstruction();
    bool isEndOfInstruction(const Token& token) const;
    bool isInstructionActive() const;
    bool wasEndOfInstruction() const;

    const Classifier* m_classifier = nullptr;
    bool m_instructionActive{false};
    bool m_instructionEnded{false};
    size_t m_braceLevel{0};

    bool hasNext() const;
    void consume(size_t size = 1);
    auto fetch(size_t offset = 0) const -> char;
    auto fetchAndConsume() -> char;
    auto position() const -> size_t;
    auto tokenStart() const -> size_t;

    const char* m_source = nullptr;
    size_t m_offset = 0;
    size_t m_sourceSize = 0;
    size_t m_tokenStart = 0;

    void reportUnexpectedCharacter();
    void reportIntegerDoesNotFit();

    diagnostics::SourceReporter* m_reporter = nullptr;
};

} // namespace fuse::assembler