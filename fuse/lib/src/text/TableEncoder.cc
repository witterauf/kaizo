#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/text/TableEncoder.h>

namespace fuse::text {

auto TableEncoder::tableCount() const -> size_t
{
    return m_tables.size();
}

bool TableEncoder::hasTable(const std::string& name) const
{
    return m_tables.cend() !=
           std::find_if(m_tables.cbegin(), m_tables.cend(),
                        [&name](auto const& table) { return table.name() == name; });
}

auto TableEncoder::activeTable() const -> const Table&
{
    return m_tables[m_activeTable];
}

void TableEncoder::addTable(Table&& table)
{
    Expects(!hasTable(table.name()));
    m_tables.push_back(std::move(table));
}

void TableEncoder::setActiveTable(size_t index)
{
    Expects(index < tableCount());
    m_activeTable = index;
}

void TableEncoder::setActiveTable(const std::string& name)
{
    Expects(hasTable(name));
    for (auto i = 0U; i < tableCount(); ++i)
    {
        if (m_tables[i].name() == name)
        {
            m_activeTable = i;
            return;
        }
    }
}

void TableEncoder::setFixedLength(size_t length)
{
    m_fixedLength = length;
}

void TableEncoder::unsetFixedLength()
{
    m_fixedLength = {};
}

auto TableEncoder::encode(const std::string& text) -> Binary
{
    m_text = &text;
    m_index = 0;
    m_binary.clear();

    while (m_index < text.length())
    {
        if (text[m_index] == '{')
        {
            if (encodeControl())
            {
                return std::move(m_binary);
            }
        }
        else
        {
            encodeCharacters();
        }
    }
    return std::move(m_binary);
}

void TableEncoder::encodeCharacters()
{
    if (auto maybeEntry =
            activeTable().findLongestTextMatch(m_text->begin() + m_index, m_text->end()))
    {
        for (auto c : maybeEntry->binary())
        {
            m_binary.append(static_cast<uint8_t>(c));
        }
        m_index += maybeEntry->text().text().length();
    }
    else
    {
        // TODO: check other tables
        throw std::runtime_error{"no encoding found"};
    }
}

bool TableEncoder::encodeControl()
{
    consume();
    if (auto maybeLabel = parseLabel())
    {
        if (auto maybeArguments = parseArguments())
        {
        }
    }
    return false;
}

auto TableEncoder::parseLabel() -> std::optional<std::string>
{
    std::string label;
    while (fetch() != ':' && fetch() != '}')
    {
        if (m_index >= textLength())
        {
            return {};
        }
        else
        {
            label += fetch();
            ++m_index;
        }
    }
    return label;
}

auto TableEncoder::parseArguments() -> std::optional<std::vector<long>>
{
    std::vector<long> arguments;
    while (fetch() != '}')
    {
        if (m_index >= textLength())
        {
            return {};
        }
        else
        {
        }
    }
    return arguments;
}

auto TableEncoder::parseArgument() -> std::optional<long>
{
    long argument;
    return argument;
}

} // namespace fuse::text