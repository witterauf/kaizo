#include "kaizo/text/Table.h"
#include <contracts/Contracts.h>

namespace kaizo {

void Table::setName(const std::string& name)
{
    m_name = name;
}

auto Table::name() const -> const std::string&
{
    return m_name;
}

bool Table::isAnonymous() const
{
    return m_name.empty();
}

auto Table::size() const -> size_t
{
    return m_mapping.size();
}

bool Table::hasControl(const std::string& label) const
{
    for (auto const& pair : m_mapping)
    {
        if (pair.second.isControl() && pair.second.labelName() == label)
        {
            return true;
        }
    }
    return false;
}

auto Table::control(const std::string& label) const -> std::optional<EntryReference>
{
    auto iter = m_control.find(label);
    if (iter != m_control.cend())
    {
        auto const entry = m_mapping.find(iter->second);
        return EntryReference{&entry->first, &entry->second};
    }
    return {};
}

void Table::insert(const BinarySequence& binary, const TableEntry& entry)
{
    m_mapping.insert(std::make_pair(binary, entry));
    if (entry.isText())
    {
        m_textMapping.insert(std::make_pair(entry.text(), binary));
    }
    else
    {
        m_control.insert(std::make_pair(entry.label().name, binary));
    }
}

auto Table::entry(size_t index) const -> EntryReference
{
    Expects(index < size());
    auto iter = m_mapping.begin();
    std::advance(iter, index);
    return EntryReference{&iter->first, &iter->second};
}

} // namespace kaizo
