#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/binary/RecordData.h>
#include <fuse/binary/RecordFormat.h>

namespace fuse::binary {

void RecordFormat::append(const std::string& name, std::unique_ptr<DataFormat>&& data)
{
    m_elements.push_back(Element{name, std::move(data)});
}

auto RecordFormat::elementCount() const -> size_t
{
    return m_elements.size();
}

auto RecordFormat::has(const std::string& name)
{
    return m_elements.cend() !=
           std::find_if(m_elements.cbegin(), m_elements.cend(),
                        [&name](auto const& element) { return element.name == name; });
}

auto RecordFormat::element(size_t index) const -> const Element&
{
    return m_elements[index];
}

auto RecordFormat::element(const std::string& name) -> const Element&
{
    Expects(has(name));
    auto iter = std::find_if(m_elements.cbegin(), m_elements.cend(),
                             [&name](auto const& element) { return element.name == name; });
    return *iter;
}

auto RecordFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    auto record = std::make_unique<RecordData>();
    for (auto const& element : m_elements)
    {
        if (auto data = element.format->decode(reader))
        {
            record->set(element.name, std::move(data));
        }
        else
        {
            return {};
        }
    }
    return std::move(record);
}

} // namespace fuse::binary