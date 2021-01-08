#include <fuse/binary/data/StringData.h>

namespace kaizo::data {

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
    return std::unique_ptr<StringData>{new StringData{*this}};
}

} // namespace kaizo::data