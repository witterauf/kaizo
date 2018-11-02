#include "TableControlParser.h"
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
    // TODO: actually implement optimal-path algorithm

    if (auto maybeEntry =
            activeTable().findLongestTextMatch(m_text->begin() + m_index, m_text->end()))
    {
        m_binary.append(maybeEntry->binary());
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
    TableControlParser parser;
    if (auto maybeControl = parser.parse(*m_text, m_index))
    {
        return encodeControl(maybeControl->label, maybeControl->arguments);
    }
    return false;
}

bool TableEncoder::encodeControl(const std::string& label, const std::vector<long>& arguments)
{
    if (auto maybeControl = activeTable().control(label))
    {
        m_binary.append(maybeControl->binary());

        auto const& control = maybeControl->text();
        if (arguments.size() != control.parameterCount())
        {
            return false;
        }
        for (auto i = 0U; i < arguments.size(); ++i)
        {
            auto const parameter = arguments[i];
            auto const parameterFormat = control.parameter(i);

            if (parameterFormat.isCompatible(parameter))
            {
                auto binary = parameterFormat.encode(parameter);
                m_binary.append(binary);
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

} // namespace fuse::text