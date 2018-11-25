#include <fuse/binary/data/Data.h>

namespace fuse::binary {

Data::Data(DataType type)
    : m_type{type}
{
}

auto Data::type() const -> DataType
{
    return m_type;
}

} // namespace fuse::binary