#include "TableControlParser.h"
#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/text/TableEncoder.h>
#include <iostream>

namespace fuse::text {

TableEncoder::TableEncoder(const Table& table)
{
    m_tables.push_back(table);
    m_activeTable = 0;
}

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
        if (auto maybeNextControl = findNextControl())
        {
            if (*maybeNextControl > m_index)
            {
                if (auto maybeBinary = encodeCharacters(m_index, *maybeNextControl))
                {
                    m_binary.append(maybeBinary->second);
                    m_index += maybeBinary->first;
                }
                else
                {
                    throw std::runtime_error{ "could not encode characters" };
                }
            }
            if (!encodeControl())
            {
                throw std::runtime_error{"could not encode control code"};
            }
        }
        else
        {
            if (auto maybeBinary = encodeCharacters(m_index, text.size()))
            {
                m_binary.append(maybeBinary->second);
                m_index += maybeBinary->first;
            }
            else
            {
                throw std::runtime_error{"could not encode characters"};
            }
        }
    }
    return std::move(m_binary);
}

auto TableEncoder::encodeCharacters(size_t begin, size_t end)
    -> std::optional<std::pair<size_t, BinarySequence>>
{
    if (auto maybeEntries =
            activeTable().findNextTextMatches(m_text->begin() + begin, m_text->begin() + end))
    {
        for (auto const& entry : *maybeEntries)
        {
            auto const length = entry.text().text().length();
            auto const newBegin = begin + length;
            if (newBegin == end)
            {
                return std::make_pair(length, entry.binary());
            }
            if (auto maybeBinary = encodeCharacters(newBegin, end))
            {
                return std::make_pair(length + maybeBinary->first,
                                      entry.binary() + maybeBinary->second);
            }
        }
    }
    return {};
}

bool TableEncoder::encodeControl()
{
    TableControlParser parser;
    if (auto maybeControl = parser.parse(*m_text, m_index))
    {
        if (encodeControl(maybeControl->label, maybeControl->arguments))
        {
            m_index = maybeControl->offset;
            return true;
        }
    }
    return false;
}

bool TableEncoder::encodeControl(
    const std::string& label, const std::vector<TableEntry::ParameterFormat::argument_t>& arguments)
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

auto TableEncoder::findNextControl() const -> std::optional<size_t>
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

auto TableEncoder::textLength() const -> size_t
{
    return m_text->length();
}

} // namespace fuse::text