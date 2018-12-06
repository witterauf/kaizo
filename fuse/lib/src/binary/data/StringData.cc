#include <fuse/binary/data/StringData.h>

namespace fuse::binary {

StringData::StringData()
    : Data{DataType::String}
{
}

StringData::StringData(const std::string& value)
    : Data{DataType::String}
    , m_value{value}
{
}

void StringData::setValue(const std::string& value)
{
    m_value = value;
}

auto StringData::value() const -> const std::string&
{
    return m_value;
}

bool StringData::isEqual(const Data& rhs) const
{
    if (rhs.type() == type())
    {
        return m_value == static_cast<const StringData&>(rhs).m_value;
    }
    return false;
}

auto StringData::copy() const -> std::unique_ptr<Data>
{
    auto data = std::make_unique<StringData>();
    data->m_value = m_value;
    return std::move(data);
}

} // namespace fuse::binary