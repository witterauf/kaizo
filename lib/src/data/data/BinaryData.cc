#include <kaizo/data/data/BinaryData.h>

namespace kaizo::data {

BinaryData::BinaryData()
    : Data{DataType::Binary}
{
}

auto BinaryData::size() const -> size_t
{
    return m_data.size();
}

void BinaryData::setData(const Binary& binary)
{
    m_data = binary;
}

void BinaryData::setData(Binary&& binary)
{
    m_data = std::move(binary);
}

auto BinaryData::data() const -> const Binary&
{
    return m_data;
}

bool BinaryData::isEqual(const Data& rhs) const
{
    if (rhs.type() == type())
    {
        return m_data == static_cast<const BinaryData&>(rhs).m_data;
    }
    return false;
}

auto BinaryData::copy() const -> std::unique_ptr<Data>
{
    auto data = std::make_unique<BinaryData>();
    data->m_data = m_data;
    return std::move(data);
}

} // namespace kaizo::data