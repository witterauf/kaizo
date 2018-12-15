#include <fuse/binary/UnresolvedReference.h>

namespace fuse::binary {

UnresolvedReference::UnresolvedReference(const DataPath& path, size_t offset)
    : m_sourcePath{path}
    , m_relativeOffset{offset}
{
}

bool UnresolvedReference::isValid() const
{
    return m_format != nullptr;
}

void UnresolvedReference::setDestination(const DataPath& path)
{
    m_destinationPath = path;
}

void UnresolvedReference::setFormat(const std::shared_ptr<AddressStorageFormat>& format)
{
    m_format = format;
}

} // namespace fuse::binary