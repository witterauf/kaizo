#include <diagnostics/Contracts.h>
#include <fuse/binary/RecordData.h>

namespace fuse::binary {

RecordData::RecordData()
    : Data{DataType::Record}
{
}

void RecordData::set(const std::string& name, std::unique_ptr<Data>&& data)
{
    m_elements.insert(std::make_pair(name, std::move(data)));
}

auto RecordData::elementCount() const -> size_t
{
    return m_elements.size();
}

auto RecordData::has(const std::string& name) const
{
    return m_elements.cend() != m_elements.find(name);
}

auto RecordData::element(const std::string& name) const -> const Data&
{
    Expects(has(name));
    return *m_elements.at(name);
}

auto RecordData::elementNames() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    for (auto const& pair : m_elements)
    {
        names.push_back(pair.first);
    }
    return names;
}

bool RecordData::isEqual(const Data& rhs) const
{
    if (rhs.type() != type())
    {
        auto const& recordRhs = static_cast<const RecordData&>(rhs);
        if (recordRhs.elementCount() != elementCount())
        {
            return false;
        }
        for (auto const& pair : m_elements)
        {
            auto const& name = pair.first;
            auto const& data = *pair.second;
            if (!recordRhs.has(name))
            {
                return false;
            }
            if (!data.isEqual(recordRhs.element(name)))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

auto RecordData::copy() const -> std::unique_ptr<Data>
{
    auto data = std::make_unique<RecordData>();
    for (auto const& pair : m_elements)
    {
        auto const& name = pair.first;
        auto const& element = *pair.second;
        data->set(name, element.copy());
    }
    return std::move(data);
}

} // namespace fuse::binary