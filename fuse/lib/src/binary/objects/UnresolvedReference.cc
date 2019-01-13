#include <diagnostics/Contracts.h>
#include <fuse/binary/objects/UnresolvedReference.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>
#include <fuse/utilities/DomReaderHelpers.h>

using namespace fuse::binary;

namespace fuse {

auto UnresolvedReference::deserialize(LuaDomReader& reader) -> UnresolvedReference
{
    Expects(reader.isRecord());
    auto const offset = requireUnsignedInteger(reader, "offset");
    auto const pathString = requireString(reader, "referenced");
    DataPath path;
    if (auto maybePath = DataPath::fromString(pathString))
    {
        path = *maybePath;
    }
    else
    {
        throw FuseException{"'" + pathString + "' is not a valid path"};
    }

    enterRecord(reader, "format");
    auto format = AddressStorageFormat::deserialize(reader);
    reader.leave();

    UnresolvedReference reference{offset};
    reference.setFormat(std::move(format));
    reference.setDestination(path);
    return std::move(reference);
}

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

void UnresolvedReference::setFormat(const std::shared_ptr<AddressStorageFormat>& format)
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

auto UnresolvedReference::addressLayout() const -> const AddressStorageFormat&
{
    Expects(m_format);
    return *m_format;
}

void UnresolvedReference::serialize(LuaWriter& writer) const
{
    writer.startTable()
        .startField("offset")
        .writeInteger(m_relativeOffset)
        .finishField()
        .startField("referenced")
        .writeString(m_destinationPath.toString())
        .finishField()
        .startField("format");
    m_format->serialize(writer);
    writer.finishField().finishTable();
}

} // namespace fuse