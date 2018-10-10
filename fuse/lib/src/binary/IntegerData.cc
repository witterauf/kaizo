#include <fuse/binary/IntegerData.h>

namespace fuse::binary {

IntegerData::IntegerData()
    : Data{DataType::Integer}
    , m_value{0ULL}
{
}

IntegerData::IntegerData(int64_t value)
    : Data{DataType::Integer}
    , m_value{value}
{
}

IntegerData::IntegerData(uint64_t value)
    : Data{DataType::Integer}
    , m_value{value}
{
}

bool IntegerData::isEqual(const Data& rhs) const
{
    if (rhs.type() == type())
    {
        return m_value == static_cast<const IntegerData&>(rhs).m_value;
    }
    return false;
}

auto IntegerData::copy() const -> std::unique_ptr<Data>
{
    auto data = std::make_unique<IntegerData>();
    data->m_value = m_value;
    return std::move(data);
}

} // namespace fuse::binary
