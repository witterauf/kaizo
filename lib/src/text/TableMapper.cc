#include "kaizo/text/TableMapper.h"
#include "TableControlParser.h"
#include <algorithm>
#include <contracts/Contracts.h>

namespace kaizo {

TableMapper::TableMapper(const Table& table)
{
    m_tables.push_back(table);
    m_activeTable = 0;
}

auto TableMapper::tableCount() const -> size_t
{
    return m_tables.size();
}

bool TableMapper::hasTable(const std::string& name) const
{
    return m_tables.cend() !=
           std::find_if(m_tables.cbegin(), m_tables.cend(),
                        [&name](auto const& table) { return table.name() == name; });
}

auto TableMapper::activeTable() const -> const Table&
{
    Expects(m_activeTable < m_tables.size());
    return m_tables[m_activeTable];
}

void TableMapper::addTable(Table&& table)
{
    Expects(!hasTable(table.name()));
    m_tables.push_back(std::move(table));
}

void TableMapper::addTable(const Table& table)
{
    Expects(!hasTable(table.name()));
    m_tables.push_back(table);
}

void TableMapper::setActiveTable(size_t index)
{
    Expects(index < tableCount());
    m_activeTable = index;
}

void TableMapper::setActiveTable(const std::string& name)
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

void TableMapper::setMapper(Mapper mapper)
{
    m_mapper = mapper;
}

void TableMapper::map(const std::string& text)
{
    m_text = &text;
    m_index = 0;

    while (m_index < text.length())
    {
        if (auto maybeNextControl = findNextControl())
        {
            if (*maybeNextControl > m_index)
            {
                tryMappingCharacters(m_index, *maybeNextControl);
            }
            if (!mapControl())
            {
                throw std::runtime_error{"could not encode control code"};
            }
        }
        else
        {
            tryMappingCharacters(m_index, m_text->length());
        }
    }
}

void TableMapper::tryMappingCharacters(size_t begin, size_t end)
{
    auto entries = mapCharacters(begin, end);
    if (!entries.empty())
    {
        for (auto const& entry : entries)
        {
            Mapping mapping;
            mapping.entry = entry;
            map(entry.text().text(), mapping);
        }
        m_index += end - begin;
    }
    else
    {
        std::string textSnippet;
        if (begin > 0)
        {
            textSnippet += "... ";
        }
        textSnippet += m_text->substr(begin, end - begin);
        if (end < m_text->length())
        {
            textSnippet += " ...";
        }

        throw std::runtime_error{"could not map '" + textSnippet + "'"};
    }
}

auto TableMapper::mapCharacters(size_t begin, size_t end) -> std::vector<Table::EntryReference>
{
    std::vector<Table::EntryReference> entries;
    mapCharacters(begin, end, entries);
    return entries;
}

bool TableMapper::mapCharacters(size_t begin, size_t end,
                                std::vector<Table::EntryReference>& entries)
{
    if (auto maybeEntries =
            activeTable().findNextTextMatches(m_text->begin() + begin, m_text->begin() + end))
    {
        for (auto const& entry : *maybeEntries)
        {
            auto const length = entry.text().text().length();
            auto const newBegin = begin + length;
            entries.push_back(entry);
            if (newBegin == end)
            {
                return true;
            }
            else if (mapCharacters(newBegin, end, entries))
            {
                return true;
            }
            else
            {
                entries.pop_back();
            }
        }
    }
    return false;
}

bool TableMapper::mapControl()
{
    TableControlParser parser{&m_tables[m_activeTable]};
    if (auto maybeControl = parser.parse(*m_text, m_index))
    {
        auto const originalText = m_text->substr(m_index, maybeControl->offset - m_index);
        Mapping mapping;
        mapping.entry = maybeControl->entry;
        mapping.arguments = maybeControl->arguments.controlArguments;
        map(originalText, mapping);
        m_index = maybeControl->offset;
        return true;
    }
    else
    {
        throw std::runtime_error{
            "could not parse control code " +
            m_text->substr(m_index, std::min(16ULL, m_text->length() - m_index))};
    }
}

auto TableMapper::findNextControl() const -> std::optional<size_t>
{
    for (auto i = m_index; i < textLength(); ++i)
    {
        if ((*m_text)[i] == '{')
        {
            return i;
        }
    }
    return {};
}

auto TableMapper::textLength() const -> size_t
{
    Expects(m_text);
    return m_text->length();
}

bool TableMapper::map(const std::string& text, const Mapping& mapping)
{
    return m_mapper(text, mapping);
}

} // namespace kaizo