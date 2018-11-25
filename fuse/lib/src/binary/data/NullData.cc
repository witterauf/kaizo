#include <fuse/binary/data/NullData.h>

namespace fuse::binary {

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

} // namespace fuse::binary