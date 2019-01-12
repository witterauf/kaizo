#include <diagnostics/Contracts.h>
#include <functional>
#include <fuse/FuseException.h>
#include <fuse/binary/objects/Object.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>

namespace fuse {

class ObjectSectionDeserializer : public DeserializationConsumer
{
public:
    ObjectSectionDeserializer(LuaReader* reader)
        : m_reader{reader}
    {
    }

    void enterRecord() override;
    bool enterField(const std::string& name) override;
    void consumeInteger(int64_t value) override;

    auto section() const -> Object::Section
    {
        return m_section;
    }

private:
    bool m_isRecord{false};
    LuaReader* m_reader;
    Object::Section m_section;
    std::function<void(size_t)> m_setter;
};

void ObjectSectionDeserializer::enterRecord()
{
    m_isRecord = true;
    m_section = {};
}

bool ObjectSectionDeserializer::enterField(const std::string& name)
{
    Expects(m_isRecord);
    auto* section = &m_section;
    if (name == "offset")
    {
        m_setter = [section](size_t value) { section->offset = value; };
    }
    else if (name == "size")
    {
        m_setter = [section](size_t value) { section->size = value; };
    }
    else if (name == "actual_offset")
    {
        m_setter = [section](size_t value) { section->realOffset = value; };
    }
    else
    {
        throw FuseException{"deserialization error: unknown field '" + name + "'"};
    }
    return true;
}

void ObjectSectionDeserializer::consumeInteger(int64_t value)
{
    Expects(m_isRecord);
    Expects(m_setter);
    if (value < 0)
    {
        throw FuseException{"deserialization error: invalid value for section field"};
    }
    m_setter(static_cast<size_t>(value));
}

class PackedObjectDeserializer : public DeserializationConsumer
{
public:
    PackedObjectDeserializer(LuaReader* reader, AnnotatedBinary* parent)
        : m_reader{reader}
        , m_parent{parent}
    {
    }

    void enterArray(size_t size) override;
    void enterRecord() override;
    bool enterField(const std::string& name) override;
    bool enterElement(size_t index) override;
    void consumeInteger(int64_t value) override;
    void consumeString(const char* value) override;
    void leaveElement() override;
    void leaveRecord() override;
    void leaveArray() override;

    auto object() -> std::unique_ptr<PackedObject>;

private:
    enum class CurrentField
    {
        Root,
        Path,
        Offset,
        Size,
        ActualSize,
        Sections,
        UnresolvedReferences
    };

    AnnotatedBinary* m_parent{nullptr};
    LuaReader* m_reader{nullptr};
    CurrentField m_currentField{CurrentField::Root};
    PackedObject* m_object{nullptr};
};

void PackedObjectDeserializer::enterArray(size_t)
{
    if (m_currentField != CurrentField::Sections &&
        m_currentField != CurrentField::UnresolvedReferences)
    {
        throw FuseException{"deserialization error: unexpected array"};
    }
}

void PackedObjectDeserializer::enterRecord()
{
    Expects(m_currentField == CurrentField::Root);
    m_object = new PackedObject{};
    m_object->m_parent = m_parent;
}

bool PackedObjectDeserializer::enterField(const std::string& name)
{
    if (name == "path")
    {
        m_currentField = CurrentField::Path;
    }
    else if (name == "offset")
    {
        m_currentField = CurrentField::Offset;
    }
    else if (name == "size")
    {
        m_currentField = CurrentField::Size;
    }
    else if (name == "actual_size")
    {
        m_currentField = CurrentField::ActualSize;
    }
    else if (name == "unresolved_references")
    {
        m_currentField = CurrentField::UnresolvedReferences;
    }
    else if (name == "sections")
    {
        m_currentField = CurrentField::Sections;
    }
    else
    {
        throw FuseException{"deserialization error: unknown field + " + name};
    }
    return true;
}

bool PackedObjectDeserializer::enterElement(size_t)
{
    if (m_currentField == CurrentField::Sections)
    {
        ObjectSectionDeserializer sectionDeserializer{m_reader};
        m_reader->deserialize(&sectionDeserializer);
        auto const section = sectionDeserializer.section();
        m_object->addSection(section.realOffset, section.size);
    }
    else
    {
        auto reference = UnresolvedReference::deserialize(*m_reader);
        m_object->addUnresolvedReference(reference);
    }
    return false;
}

void PackedObjectDeserializer::consumeInteger(int64_t value)
{
    switch (m_currentField)
    {
    case CurrentField::Offset: m_object->changeOffset(static_cast<size_t>(value)); return;
    default: return;
    }
}

void PackedObjectDeserializer::consumeString(const char* value)
{
    if (m_currentField == CurrentField::Path)
    {
        if (auto maybePath = binary::DataPath::fromString(value))
        {
            m_object->m_path = std::move(*maybePath);
        }
    }
}

void PackedObjectDeserializer::leaveElement()
{
}

void PackedObjectDeserializer::leaveRecord()
{
}

void PackedObjectDeserializer::leaveArray()
{
}

auto PackedObjectDeserializer::object() -> std::unique_ptr<PackedObject>
{
    return std::unique_ptr<PackedObject>(m_object);
}

auto PackedObject::deserialize(LuaReader& reader, AnnotatedBinary* parent)
    -> std::unique_ptr<PackedObject>
{
    PackedObjectDeserializer deserializer{&reader, parent};
    reader.deserialize(&deserializer);
    return deserializer.object();
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
    Expects(index < m_references.size());
    return m_references[index];
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