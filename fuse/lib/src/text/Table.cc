#include <diagnostics/Contracts.h>
#include <fuse/text/Table.h>

namespace fuse::text {

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
    for (auto const& pair : m_mapping)
    {
        if (pair.second.isControl() && pair.second.labelName() == label)
        {
            return EntryReference{&pair.first, &pair.second};
        }
    }
    return {};
}

void Table::insert(const BinarySequence& binary, const TableEntry& text)
{
    m_mapping.insert(std::make_pair(binary, text));
}

auto Table::entry(size_t index) const -> EntryReference
{
    Expects(index < size());
    auto iter = m_mapping.begin();
    std::advance(iter, index);
    return EntryReference{ &iter->first, &iter->second };
}

} // namespace fuse::text
