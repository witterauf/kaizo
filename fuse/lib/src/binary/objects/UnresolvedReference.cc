#include <contracts/Contracts.h>
#include <fuse/binary/objects/UnresolvedReference.h>
#include <fuse/utilities/DomReaderHelpers.h>

using namespace fuse::binary;

namespace fuse {

UnresolvedReference::UnresolvedReference(size_t offset)
    : m_relativeOffset{offset}
{
}

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

void UnresolvedReference::setFormat(const std::shared_ptr<AddressLayout>& format)
{
    m_format = format;
}

auto UnresolvedReference::originPath() const -> const binary::DataPath&
{
    return m_sourcePath;
}

auto UnresolvedReference::referencedPath() const -> const binary::DataPath&
{
    return m_destinationPath;
}

auto UnresolvedReference::relativeOffset() const -> size_t
{
    return m_relativeOffset;
}

auto UnresolvedReference::addressLayout() const -> const AddressLayout&
{
    Expects(m_format);
    return *m_format;
}

} // namespace fuse