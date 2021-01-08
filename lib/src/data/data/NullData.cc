#include <kaizo/data/data/NullData.h>

namespace kaizo::data {

NullData::NullData()
    : Data{DataType::Null}
{
}

bool NullData::isEqual(const Data& rhs) const
{
    return rhs.type() == DataType::Null;
}

auto NullData::copy() const -> std::unique_ptr<Data>
{
    return std::make_unique<NullData>();
}

} // namespace kaizo::data