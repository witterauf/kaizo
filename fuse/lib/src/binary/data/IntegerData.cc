#include <fuse/binary/data/IntegerData.h>

namespace kaizo::data::binary {

IntegerData::IntegerData()
    : Data{DataType::Integer}
    , m_value{0ULL}
{
}

IntegerData::IntegerData(const int64_t value)
    : Data{DataType::Integer}
    , m_value{value}
{
}

IntegerData::IntegerData(const uint64_t value)
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

IntegerData::IntegerData(const IntegerData& other)
    : Data{other}
{
}

auto IntegerData::copy() const -> std::unique_ptr<Data>
{
    return std::unique_ptr<IntegerData>{new IntegerData{*this}};
}

bool IntegerData::isNegative() const
{
    if (std::holds_alternative<int64_t>(m_value) && std::get<int64_t>(m_value) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

auto IntegerData::asUnsigned() const -> uint64_t
{
    if (std::holds_alternative<int64_t>(m_value))
    {
        return static_cast<uint64_t>(std::get<int64_t>(m_value));
    }
    else
    {
        return std::get<uint64_t>(m_value);
    }
}

auto IntegerData::asSigned() const -> int64_t
{
    if (std::holds_alternative<uint64_t>(m_value))
    {
        return static_cast<int64_t>(std::get<uint64_t>(m_value));
    }
    else
    {
        return std::get<int64_t>(m_value);
    }
}

auto IntegerData::requiredSize() const -> size_t
{
    size_t size{1};
    if (isNegative())
    {
        return 0;
    }
    else
    {
        auto const value = asUnsigned();
        while (value >= (1ULL << (size * 8)))
        {
            if (++size == 8)
            {
                return 8;
            }
        }
    }
    return size;
}

} // namespace kaizo::data::binary
