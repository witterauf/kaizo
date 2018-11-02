#include "TableControlParser.h"
#include <cctype>
#include <diagnostics/Contracts.h>

namespace fuse::text {

auto TableControlParser::parse(const std::string& text, size_t index) -> std::optional<ControlCode>
{
    setSource(text, index);
    return parseControl();
}

void TableControlParser::setSource(const std::string& text, size_t index)
{
    Expects(index < text.length());
    m_text = &text;
    m_index = index;
}

auto TableControlParser::parseControl() -> std::optional<ControlCode>
{
    consume(); // '{'
    if (auto maybeLabel = parseLabel())
    {
        if (auto maybeArguments = parseArguments())
        {
            if (expectAndConsume('}'))
            {
                return ControlCode{*maybeLabel, *maybeArguments};
            }
        }
    }
    return {};
}

auto TableControlParser::parseLabel() -> std::optional<std::string>
{
    std::string label;
    while (fetch() != ':' && fetch() != '}')
    {
        if (!hasNext())
        {
            return {};
        }
        else
        {
            label += fetch();
            consume();
        }
    }
    return label;
}

auto TableControlParser::parseArguments() -> std::optional<std::vector<long>>
{
    std::vector<long> arguments;
    if (auto maybeArgument = parseArgument())
    {
        arguments.push_back(*maybeArgument);
    }
    else
    {
        return {};
    }

    while (fetch() == ',')
    {
        if (auto maybeArgument = parseArgument())
        {
            arguments.push_back(*maybeArgument);
        }
        else
        {
            return {};
        }
    }
    return arguments;
}

auto TableControlParser::parseArgument() -> std::optional<long>
{
    bool negative{false};
    if (fetchThenConsume('-'))
    {
        negative = true;
    }
    long argument;
    if (fetch(0) == '0' && fetch(1) == 'x')
    {
        consume(2);
        if (auto maybeArgument = parseHexadecimalArgument())
        {
            argument = *maybeArgument;
        }
        else
        {
            return {};
        }
    }
    else
    {
        if (auto maybeArgument = parseDecimalArgument())
        {
            argument = *maybeArgument;
        }
        else
        {
            return {};
        }
    }
    return negative ? -argument : argument;
}

static auto hexadecimalDigitValue(char c) -> long
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<long>(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return static_cast<long>(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
        return static_cast<long>(c - 'A' + 10);
    }
    throw std::logic_error{"should never happen"};
}

auto TableControlParser::parseHexadecimalArgument() -> std::optional<long>
{
    long argument{0};
    if (!std::isxdigit(fetch()))
    {
        return {};
    }
    while (std::isxdigit(fetch()))
    {
        argument *= 16;
        argument += hexadecimalDigitValue(fetchAndConsume());
    }
    return argument;
}

auto TableControlParser::parseDecimalArgument() -> std::optional<long>
{
    long argument{0};
    if (!std::isdigit(fetch()))
    {
        return {};
    }
    while (std::isdigit(fetch()))
    {
        argument *= 10;
        argument += static_cast<long>(fetchAndConsume() - '0');
    }
    return argument;
}

bool TableControlParser::hasNext() const
{
    return m_index < m_text->length();
}

bool TableControlParser::expectAndConsume(char c)
{
    if (fetch() == c)
    {
        consume();
        return true;
    }
    else
    {
        return false;
    }
}

auto TableControlParser::fetch(size_t offset) -> char
{
    if (hasNext())
    {
        return m_text->at(offset);
    }
    else
    {
        return '\0';
    }
}

bool TableControlParser::fetchThenConsume(char c)
{
    if (fetch() == c)
    {
        consume();
        return true;
    }
    return false;
}

auto TableControlParser::fetchAndConsume() -> char
{
    auto const c = fetch();
    consume();
    return c;
}

void TableControlParser::consume(size_t size)
{
    m_index += size;
}

} // namespace fuse::text