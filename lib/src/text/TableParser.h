#pragma once

#include <fuse/text/Table.h>
#include <optional>

namespace diagnostics {
class SourceReporter;
}

namespace fuse::text {

class TableParser
{
public:
    void setReporter(diagnostics::SourceReporter* reporter);
    bool hasReporter();
    auto reporter() -> diagnostics::SourceReporter&;

    auto read(const char* source, size_t size) -> std::optional<Table>;

    void setSource(const char* source, size_t size);
    auto parseTable() -> std::optional<Table>;
    bool parseTableName(Table& table);
    bool parseTableEntries(Table& table);
    bool parseTableEntry(Table& table);
    bool parseTableSwitchEntry(Table& table);
    bool parseEndEntry(Table& table);
    bool parseControlEntry(Table& table);
    bool parseTextEntry(Table& table);
    auto parseLabel() -> std::optional<TextSequence::Label>;
    auto parseLabelName() -> std::optional<std::string>;
    auto parseText() -> std::optional<std::string>;
    auto parseBinarySequence() -> std::optional<BinarySequence>;
    auto parseParameter() -> std::optional<TextSequence::Parameter>;
    auto parseLineBreaks() -> std::optional<size_t>;

private:
    bool hasNext() const;
    auto fetch(size_t offset = 0) const -> char;
    auto fetchAndConsume() -> char;
    void consume(size_t size = 1);
    bool expectAndConsume(char c);
    void skipLineBreaks();

    const char* m_source{nullptr};
    size_t m_sourceSize;
    size_t m_index;

    void reportUnexpectedCharacter();
    void reportUnexpectedCharacter(char expected);
    void reportUnexpectedEndOfLine();
    void reportUnexpectedEndOfFile();
    void reportIncompleteHexDigit();
    void reportExpectedParameterName();
    void reportUnknownParameterFormat();

    diagnostics::SourceReporter* m_reporter{nullptr};
};

} // namespace fuse::text