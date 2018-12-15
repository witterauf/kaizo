#include <fuse/binary/data/ReferenceData.h>

namespace fuse::binary {

ReferenceData::ReferenceData(const DataPath& path)
    : Data{DataType::Reference}
    , m_path{path}
{
}

auto ReferenceData::path() const -> const DataPath&
{
    return m_path;
}

bool ReferenceData::isEqual(const Data& rhs) const
{
    if (type() != rhs.type())
    {
        return false;
    }
    auto const referenceRhs = static_cast<const ReferenceData&>(rhs);
    return path() == referenceRhs.path();
}

auto ReferenceData::copy() const -> std::unique_ptr<Data>
{
    return std::make_unique<ReferenceData>(path());
}

} // namespace fuse::binary