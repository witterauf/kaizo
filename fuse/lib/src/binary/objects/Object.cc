#include <diagnostics/Contracts.h>
#include <fuse/binary/objects/Object.h>
#include <fuse/lua/LuaWriter.h>

namespace fuse {

Object::Object(const binary::DataPath& path, size_t offset)
    : m_path{path}
    , m_offset{offset}
{
}

void Object::addSection(size_t realOffset, size_t sectionSize)
{
    Section section;
    section.offset = size();
    section.realOffset = realOffset;
    section.size = sectionSize;
    m_sections.push_back(section);
}

void Object::addUnresolvedReference(const UnresolvedReference& reference)
{
    m_references.push_back(reference);
}

auto Object::path() const -> const binary::DataPath&
{
    return m_path;
}

auto Object::realSize() const -> size_t
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

auto Object::size() const -> size_t
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

void Object::changeOffset(size_t offset)
{
    m_offset = offset;
}

auto Object::offset() const -> size_t
{
    return m_offset;
}

auto Object::sectionCount() const -> size_t
{
    return m_sections.size();
}

auto Object::section(size_t index) const -> const Section&
{
    Expects(index < sectionCount());
    return m_sections[index];
}

auto Object::unresolvedReferenceCount() const -> size_t
{
    return m_references.size();
}

auto Object::unresolvedReference(size_t index) const -> const UnresolvedReference&
{
    Expects(index < m_references.size());
    return m_references[index];
}

void Object::serialize(LuaWriter& writer) const
{
    writer.startTable();
    serializeAttributes(writer);
    serializeSections(writer);
    serializeReferences(writer);
    writer.finishTable();
}

void Object::serializeAttributes(LuaWriter& writer) const
{
    writer.startField("name").writeString(m_path.toString()).finishField();
    writer.startField("offset").writeInteger(m_offset).finishField();
}

void Object::serializeSections(LuaWriter& writer) const
{
    writer.startField("sections").startTable();
    for (auto i = 0U; i < sectionCount(); ++i)
    {
        auto const& section = m_sections[i];
        writer.startField().startTable();
        writer.startField("offset").writeInteger(section.offset).finishField();
        writer.startField("real_offset").writeInteger(section.realOffset).finishField();
        writer.startField("size").writeInteger(section.size).finishField();
        writer.finishTable().finishField();
    }
    writer.finishTable().finishField();
}

void Object::serializeReferences(LuaWriter& writer) const
{
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