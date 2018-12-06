#include "DataPathParser.h"
#include <cctype>

namespace fuse {

auto DataPathParser::parse(const std::string& string) -> std::optional<binary::DataPath>
{
    m_string = &string;
    m_index = 0;

    binary::DataPath path;
    while (hasNext())
    {
        if (auto maybeElement = parseElement())
        {
            path /= *maybeElement;
        }
        else
        {
            return {};
        }
    }
    return path;
}

auto DataPathParser::parseElement() -> std::optional<binary::DataPathElement>
{
    if (fetch() == '.')
    {
        consume();
        return parseNameElement();
    }
    else if (isFirst() && std::isalpha(fetch()))
    {
        return parseNameElement();
    }
    else if (fetch() == '[')
    {
        return parseIndexElement();
    }
    else
    {
        return {};
    }
}

auto DataPathParser::parseNameElement() -> std::optional<binary::DataPathElement>
{
    if (!std::isalpha(fetch()))
    {
        return {};
    }
    std::string name;
    while (std::isalnum(fetch()))
    {
        name += fetch();
        consume();
    }
    return binary::DataPathElement::makeName(name);
}

static auto digitValue(char c) -> size_t
{
    return static_cast<size_t>(c - '0');
}

auto DataPathParser::parseIndexElement() -> std::optional<binary::DataPathElement>
{
    consume(); // '['

    if (!std::isdigit(fetch()))
    {
        return {};
    }
    size_t index = digitValue(fetch());
    consume();

    while (fetch() != ']')
    {
        if (std::isdigit(fetch()))
        {
            index *= 10;
            index += digitValue(fetch());
            consume();
        }
        else
        {
            return {};
        }
    }
    consume();
    return binary::DataPathElement::makeIndex(index);
}

bool DataPathParser::hasNext() const
{
    return m_index < m_string->length();
}

auto DataPathParser::fetch() const -> char
{
    return (*m_string)[m_index];
}

void DataPathParser::consume()
{
    m_index++;
}

bool DataPathParser::isFirst() const
{
    return m_index == 0;
}

} // namespace fuse