#include "fuse/binary/objects/Object.h"
#include "fuse/binary/objects/AnnotatedBinary.h"
#include <contracts/Contracts.h>
#include <functional>

using namespace kaizo::data::binary;

namespace kaizo::data {

void Object::setAlignment(const size_t alignment)
{
    Expects(alignment >= 1);
    m_alignment = alignment;
}

auto Object::alignment() const -> size_t
{
    return m_alignment;
}

void Object::setFixedOffset(const size_t offset)
{
    m_fixedOffset = offset;
}

bool Object::hasFixedOffset() const
{
    return m_fixedOffset.has_value();
}

auto Object::fixedOffset() const -> size_t
{
    return *m_fixedOffset;
}

/*
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
*/

PackedObject::PackedObject(const binary::DataPath& path, AnnotatedBinary* parent, size_t offset)
    : m_path{path}
    , m_parent{parent}
    , m_offset{offset}
{
}

auto PackedObject::binary() const -> Binary
{
    return m_parent->binary().read(m_offset, size());
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

} // namespace kaizo::data