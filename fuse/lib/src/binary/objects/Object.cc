#include <diagnostics/Contracts.h>
#include <functional>
#include <fuse/FuseException.h>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <fuse/binary/objects/Object.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>
#include <fuse/utilities/DomReaderHelpers.h>

using namespace fuse::binary;

namespace fuse {

static auto deserializeSection(LuaDomReader& reader) -> Object::Section
{
    Expects(reader.isRecord());
    Object::Section section;
    section.offset = requireUnsignedInteger(reader, "offset");
    section.size = requireUnsignedInteger(reader, "size");
    section.realOffset = requireUnsignedInteger(reader, "actual_offset");
    return section;
}

auto PackedObject::deserialize(LuaDomReader& reader, AnnotatedBinary* parent)
    -> std::unique_ptr<PackedObject>
{
    Expects(reader.isRecord());

    PackedObject* object;
    if (auto maybePath = DataPath::fromString(requireString(reader, "path")))
    {
        auto const offset = requireUnsignedInteger(reader, "offset");
        object = new PackedObject{*maybePath, parent, offset};
    }
    else
    {
        throw FuseException{"invalid path"};
    }

    if (reader.has("sections"))
    {
        enterArray(reader, "sections");
        auto const size = reader.size();
        for (auto i = 0U; i < size; ++i)
        {
            reader.enter(i);
            auto section = deserializeSection(reader);
            object->m_sections.push_back(section);
            reader.leave();
        }
        reader.leave();
    }
    else
    {
        Object::Section section;
        section.offset = section.realOffset = 0;
        section.size = requireUnsignedInteger(reader, "size");
        object->m_sections.push_back(section);
    }

    if (reader.has("unresolved_references"))
    {
        enterArray(reader, "unresolved_references");
        auto const size = reader.size();
        for (auto i = 0U; i < size; ++i)
        {
            reader.enter(i);
            auto reference = UnresolvedReference::deserialize(reader);
            object->m_references.push_back(reference);
            reader.leave();
        }
        reader.leave();
    }

    return std::unique_ptr<PackedObject>(object);
}

PackedObject::PackedObject(const binary::DataPath& path, AnnotatedBinary* parent, size_t offset)
    : m_path{path}
    , m_parent{parent}
    , m_offset{offset}
{
}

void PackedObject::addSection(size_t realOffset, size_t sectionSize)
{
    Section section;
    section.offset = size();
    section.realOffset = realOffset;
    section.size = sectionSize;
    m_sections.push_back(section);
}

void PackedObject::addUnresolvedReference(const UnresolvedReference& reference)
{
    m_references.push_back(reference);
}

auto PackedObject::path() const -> const binary::DataPath&
{
    return m_path;
}

auto PackedObject::realSize() const -> size_t
{
    if (m_sections.empty())
    {
        return 0;
    }
    else
    {
        return m_sections.back().realOffset + m_sections.back().size;
    }
}

auto PackedObject::size() const -> size_t
{
    if (m_sections.empty())
    {
        return 0;
    }
    else
    {
        return m_sections.back().offset + m_sections.back().size;
    }
}

auto PackedObject::toRealOffset(size_t offset) const -> size_t
{
    for (auto const& section : m_sections)
    {
        if (offset >= section.offset && offset < section.offset + section.size)
        {
            return section.realOffset + (offset - section.offset);
        }
    }
    return 0;
}

auto PackedObject::toPackedOffset(size_t offset) const -> size_t
{
    for (auto const& section : m_sections)
    {
        if (offset >= section.realOffset && offset < section.realOffset + section.size)
        {
            return section.offset + (offset - section.realOffset);
        }
    }
    return 0;
}

void PackedObject::changeOffset(size_t offset)
{
    m_offset = offset;
}

auto PackedObject::offset() const -> size_t
{
    return m_offset;
}

auto PackedObject::sectionCount() const -> size_t
{
    return m_sections.size();
}

auto PackedObject::section(size_t index) const -> const Section&
{
    Expects(index < sectionCount());
    return m_sections[index];
}

auto PackedObject::unresolvedReferenceCount() const -> size_t
{
    return m_references.size();
}

auto PackedObject::unresolvedReference(size_t index) const -> const UnresolvedReference&
{
    Expects(index < unresolvedReferenceCount());
    return m_references[index];
}

auto PackedObject::solveReference(size_t index, const Address address) const
    -> std::vector<BinaryPatch>
{
    Expects(index < unresolvedReferenceCount());
    auto const& reference = unresolvedReference(index);
    auto patches = reference.addressLayout().writeAddress(address);
    if (patches.size() > 0)
    {
        auto const referenceOffset = reference.relativeOffset();
        auto const realReferenceOffset = toRealOffset(referenceOffset);
        for (auto& patch : patches)
        {

            patch.setRelativeOffset(toPackedOffset(realReferenceOffset + patch.relativeOffset()));
        }
    }
    return patches;
}

auto PackedObject::sectionBinary(size_t index) const -> Binary
{
    Expects(index < sectionCount());
    return m_parent->binary().read(m_offset + section(index).offset, section(index).size);
}

void PackedObject::serialize(LuaWriter& writer) const
{
    writer.startTable();
    serializeAttributes(writer);
    serializeSections(writer);
    serializeReferences(writer);
    writer.finishTable();
}

void PackedObject::serializeAttributes(LuaWriter& writer) const
{
    writer.startField("path").writeString(m_path.toString()).finishField();
    writer.startField("offset").writeInteger(m_offset).finishField();
    writer.startField("size").writeInteger(size()).finishField();
    writer.startField("actual_size").writeInteger(realSize()).finishField();
}

void PackedObject::serializeSections(LuaWriter& writer) const
{
    if (sectionCount() == 1 && section(0).realOffset == 0)
    {
        return;
    }

    writer.startField("sections").startTable();
    for (auto i = 0U; i < sectionCount(); ++i)
    {
        auto const& section = m_sections[i];
        writer.startField().startTable();
        writer.startField("offset").writeInteger(section.offset).finishField();
        writer.startField("actual_offset").writeInteger(section.realOffset).finishField();
        writer.startField("size").writeInteger(section.size).finishField();
        writer.finishTable().finishField();
    }
    writer.finishTable().finishField();
}

void PackedObject::serializeReferences(LuaWriter& writer) const
{
    if (unresolvedReferenceCount() == 0)
    {
        return;
    }

    writer.startField("unresolved_references").startTable();
    for (auto i = 0U; i < unresolvedReferenceCount(); ++i)
    {
        writer.startField();
        unresolvedReference(i).serialize(writer);
        writer.finishField();
    }
    writer.finishTable().finishField();
}

} // namespace fuse