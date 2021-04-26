#include "TableControlParser.h"
#include <cctype>
#include <contracts/Contracts.h>

namespace kaizo {

auto TableControlParser::Arguments::hook(std::string&& arg) -> Arguments
{
    Arguments arguments;
    arguments.hookArgument = std::move(arg);
    return arguments;
}

auto TableControlParser::Arguments::control(std::vector<argument_t>&& args) -> Arguments
{
    Arguments arguments;
    arguments.controlArguments = std::move(args);
    return arguments;
}

TableControlParser::TableControlParser(const Table* table)
    : m_table{table}
{
}

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
        if (auto maybeControl = m_table->control(*maybeLabel))
        {
            if (auto maybeArguments = parseArguments(maybeControl->text()))
            {
                if (expectAndConsume('}'))
                {
                    return ControlCode{m_index, *maybeControl, *maybeArguments};
                }
            }
        }
        else
        {
            throw std::runtime_error{"control code '" + *maybeLabel + "' not in table"};
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
            label += fetchAndConsume();
        }
    }
    return label;
}

auto TableControlParser::parseArguments(const TableEntry& control) -> std::optional<Arguments>
{
    switch (control.kind())
    {
    case TableEntry::Kind::Hook: return parseHookArgument();
    default: return parseArguments();
    }
}

auto TableControlParser::parseHookArgument() -> std::optional<Arguments>
{
    if (!fetchThenConsume(':'))
    {
        return Arguments::hook("");
    }

    std::string argument;
    while (fetch() != '}')
    {
        argument += fetch();
        consume();
    }
    return Arguments::hook(std::move(argument));
}

auto TableControlParser::parseArguments() -> std::optional<Arguments>
{
    if (!fetchThenConsume(':'))
    {
        return Arguments::control({});
    }

    std::vector<argument_t> arguments;
    if (auto maybeArgument = parseArgument())
    {
        arguments.push_back(*maybeArgument);
    }
    else
    {
        return {};
    }

    while (fetchThenConsume(','))
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
    return Arguments::control(std::move(arguments));
}

auto TableControlParser::parseArgument() -> std::optional<argument_t>
{
    bool negative{false};
    if (fetchThenConsume('-'))
    {
        negative = true;
    }
    argument_t argument;
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

static auto hexadecimalDigitValue(char c) -> long long
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<long long>(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return static_cast<long long>(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
        return static_cast<long long>(c - 'A' + 10);
    }
    throw std::logic_error{"should never happen"};
}

auto TableControlParser::parseHexadecimalArgument() -> std::optional<argument_t>
{
    argument_t argument{0};
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

auto TableControlParser::parseDecimalArgument() -> std::optional<argument_t>
{
    argument_t argument{0};
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
    if (m_index + offset < m_text->length())
    {
        return m_text->at(m_index + offset);
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

} // namespace kaizo