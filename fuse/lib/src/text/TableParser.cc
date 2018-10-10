#include "TableParser.h"
#include <cctype>
#include <diagnostics/Contracts.h>
#include <diagnostics/SourceReporter.h>

namespace fuse::text {

bool isLineBreak(char c)
{
    return c == '\n' || c == '\r';
}

void TableParser::setReporter(diagnostics::SourceReporter* reporter)
{
    m_reporter = reporter;
}

bool TableParser::hasReporter()
{
    return m_reporter != nullptr;
}

auto TableParser::reporter() -> diagnostics::SourceReporter&
{
    Expects(m_reporter);
    return *m_reporter;
}

auto TableParser::read(const char* source, size_t size) -> std::optional<Table>
{
    setSource(source, size);
    m_index = 0;
    return parseTable();
}

void TableParser::setSource(const char* source, size_t size)
{
    Expects(source);
    Expects(size > 0);
    m_source = source;
    m_sourceSize = size;
}

auto TableParser::parseTable() -> std::optional<Table>
{
    Table table;
    if (!parseTableName(table))
    {
        return {};
    }
    if (!parseTableEntries(table))
    {
        return {};
    }
    return table;
}

bool TableParser::parseTableName(Table& table)
{
    if (fetch() == '@')
    {
        consume();
        std::string name;
        while (!isLineBreak(fetch()))
        {
            if (fetch() == '\0')
            {
                reportUnexpectedEndOfFile();
                return {};
            }
            else if (fetch() == ',')
            {
                reportUnexpectedCharacter();
                return {};
            }

            name += fetch();
            consume();
        }
        table.setName(name);
    }
    return true;
}

bool TableParser::parseTableEntries(Table& table)
{
    while (hasNext())
    {
        if (!hasNext())
        {
            break;
        }
        if (parseTableEntry(table))
        {
            skipLineBreaks();
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool TableParser::parseTableEntry(Table& table)
{
    switch (fetch())
    {
    case '!': return parseTableSwitchEntry(table);
    case '$': return parseControlEntry(table);
    case '/': return parseEndEntry(table);
    default:
        if (std::isxdigit(fetch()))
        {
            return parseTextEntry(table);
        }
        break;
    }
    reportUnexpectedCharacter();
    return false;
}

bool TableParser::parseTableSwitchEntry(Table& table)
{
    consume();
    if (auto maybeBinary = parseBinarySequence())
    {
        if (!expectAndConsume('='))
        {
            return false;
        }
        if (auto maybeLabel = parseLabelName())
        {
            table.insert(*maybeBinary, TextSequence::makeTableSwitch(*maybeLabel));
            return true;
        }
    }
    return false;
}

auto TableParser::parseLabel() -> std::optional<TextSequence::Label>
{
    if (auto maybeLabelName = parseLabelName())
    {
        if (auto maybeLineBreaks = parseLineBreaks())
        {
            return TextSequence::Label{*maybeLabelName, *maybeLineBreaks};
        }
    }
    return {};
}

auto TableParser::parseLabelName() -> std::optional<std::string>
{
    if (!expectAndConsume('['))
    {
        return {};
    }

    std::string label;
    while (fetch() != ']')
    {
        if (fetch() == '\0')
        {
            reportUnexpectedEndOfFile();
            return {};
        }
        if (isLineBreak(fetch()))
        {
            reportUnexpectedEndOfLine();
            return {};
        }
        label += fetchAndConsume();
    }
    consume();
    return label;
}

bool TableParser::parseEndEntry(Table& table)
{
    consume();
    if (auto maybeBinary = parseBinarySequence())
    {
        if (!expectAndConsume('='))
        {
            return false;
        }
        if (auto maybeLabel = parseLabel())
        {
            table.insert(*maybeBinary, TextSequence::makeEnd(*maybeLabel));
            return true;
        }
    }
    return false;
}

bool TableParser::parseControlEntry(Table& table)
{
    consume();
    if (auto maybeBinary = parseBinarySequence())
    {
        if (!expectAndConsume('='))
        {
            return false;
        }
        if (auto maybeLabel = parseLabel())
        {
            std::vector<TextSequence::Parameter> parameters;
            while (fetch() == ',')
            {
                consume();
                if (auto maybeParameter = parseParameter())
                {
                    parameters.push_back(*maybeParameter);
                }
                else
                {
                    return {};
                }
            }
            table.insert(*maybeBinary, TextSequence::makeControl(*maybeLabel, parameters));
            return true;
        }
    }
    return false;
}

auto TableParser::parseParameter() -> std::optional<TextSequence::Parameter>
{
    TextSequence::Parameter parameter;
    while (std::isalnum(fetch()))
    {
        parameter.name += fetchAndConsume();
    }
    if (parameter.name.empty())
    {
        reportExpectedParameterName();
        return {};
    }
    if (!expectAndConsume('='))
    {
        return {};
    }
    if (!expectAndConsume('%'))
    {
        return {};
    }
    if (fetch() == 'D')
    {
        consume();
        parameter.format = TextSequence::ParameterFormat::Decimal;
    }
    else if (fetch() == 'X')
    {
        consume();
        parameter.format = TextSequence::ParameterFormat::Hexadecimal;
    }
    else if (fetch() == 'B')
    {
        consume();
        parameter.format = TextSequence::ParameterFormat::Binary;
    }
    else
    {
        reportUnknownParameterFormat();
        return {};
    }
    return parameter;
}

bool TableParser::parseTextEntry(Table& table)
{
    if (auto maybeBinary = parseBinarySequence())
    {
        if (!expectAndConsume('='))
        {
            return false;
        }
        if (auto maybeText = parseText())
        {
            table.insert(*maybeBinary, TextSequence::makeText(*maybeText));
            return true;
        }
    }
    return false;
}

auto TableParser::parseText() -> std::optional<std::string>
{
    std::string text;
    while (!isLineBreak(fetch()))
    {
        if (fetch() == '\0')
        {
            reportUnexpectedEndOfFile();
            return {};
        }
        text += fetchAndConsume();
    }
    return text;
}

static auto hexValue(char c) -> uint8_t
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<uint8_t>(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return static_cast<uint8_t>(c - 'a') + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        return static_cast<uint8_t>(c - 'A') + 10;
    }
    else
    {
        throw std::logic_error{"invalid input"};
    }
}

auto TableParser::parseBinarySequence() -> std::optional<BinarySequence>
{
    BinarySequence binary;
    while (std::isxdigit(fetch()))
    {
        if (!std::isxdigit(fetch(1)))
        {
            reportIncompleteHexDigit();
            return {};
        }
        auto const byte = (hexValue(fetch(0)) << 4) + hexValue(fetch(1));
        binary.push_back(static_cast<BinarySequence::value_type>(byte));
        consume(2);
    }
    return binary;
}

bool TableParser::hasNext() const
{
    return m_index < m_sourceSize;
}

auto TableParser::fetch(size_t offset) const -> char
{
    if (m_index + offset >= m_sourceSize)
    {
        return '0';
    }
    else
    {
        return m_source[m_index + offset];
    }
}

auto TableParser::fetchAndConsume() -> char
{
    auto const c = fetch();
    consume();
    return c;
}

void TableParser::consume(size_t size)
{
    m_index = std::min(m_sourceSize, m_index + size);
}

bool TableParser::expectAndConsume(char c)
{
    if (fetch() != c)
    {
        reportUnexpectedCharacter(c);
        return false;
    }
    else
    {
        consume();
        return true;
    }
}

auto TableParser::parseLineBreaks() -> std::optional<size_t>
{
    size_t count{0};
    while (fetch() == '\\')
    {
        if (fetch(1) == 'n')
        {
            consume(2);
            count++;
        }
        else
        {
            reportUnexpectedCharacter('n');
            return {};
        }
    }
    return count;
}

void TableParser::skipLineBreaks()
{
    while (isLineBreak(fetch()))
    {
        consume();
    }
}

//##[ diagnostics ]################################################################################

void TableParser::reportUnexpectedCharacter()
{
}

void TableParser::reportUnexpectedCharacter(char)
{
}

void TableParser::reportUnexpectedEndOfLine()
{
}

void TableParser::reportUnexpectedEndOfFile()
{
}

void TableParser::reportIncompleteHexDigit()
{
}

void TableParser::reportExpectedParameterName()
{
}

void TableParser::reportUnknownParameterFormat()
{
}

} // namespace fuse::text