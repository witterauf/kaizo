#include <diagnostics/Contracts.h>
#include <fstream>
#include <fuse/FuseException.h>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>

using namespace fuse::binary;

namespace fuse {

class AnnotatedBinaryDeserializer : public DeserializationConsumer
{
public:
    AnnotatedBinaryDeserializer(LuaReader* reader)
        : m_reader{reader}
    {
    }

    void enterArray(size_t size) override;
    void enterRecord() override;
    bool enterField(const std::string& name) override;
    bool enterElement(size_t index) override;
    void consumeString(const char* value) override;

    auto annotatedBinary() -> std::unique_ptr<AnnotatedBinary>
    {
        return std::unique_ptr<AnnotatedBinary>(m_binary);
    }

private:
    enum class CurrentField
    {
        Root,
        Binary,
        Objects
    };

    LuaReader* m_reader{nullptr};
    CurrentField m_currentField{CurrentField::Root};
    AnnotatedBinary* m_binary;
};

void AnnotatedBinaryDeserializer::enterArray(size_t)
{
    Expects(m_currentField == CurrentField::Objects);
}

void AnnotatedBinaryDeserializer::enterRecord()
{
    Expects(m_currentField == CurrentField::Root);
    m_binary = new AnnotatedBinary{};
}

bool AnnotatedBinaryDeserializer::enterField(const std::string& name)
{
    if (name == "binary")
    {
        m_currentField = CurrentField::Binary;
    }
    else if (name == "objects")
    {
        m_currentField = CurrentField::Objects;
    }
    return true;
}

bool AnnotatedBinaryDeserializer::enterElement(size_t)
{
    Expects(m_currentField == CurrentField::Objects);
    auto object = PackedObject::deserialize(*m_reader, m_binary);
    m_binary->m_objects.push_back(std::move(object));
    return false;
}

void AnnotatedBinaryDeserializer::consumeString(const char* value)
{
    Expects(m_currentField == CurrentField::Binary);
    m_binary->m_binary = Binary::load(value);
}

auto AnnotatedBinary::deserialize(LuaReader& reader) -> std::unique_ptr<AnnotatedBinary>
{
    AnnotatedBinaryDeserializer deserializer{&reader};
    reader.deserialize(&deserializer);
    return deserializer.annotatedBinary();
}

void AnnotatedBinary::startObject(const binary::DataPath& path)
{
    m_currentPath = path;
    m_currentObject = std::make_unique<PackedObject>(m_currentPath, this, m_binary.size());
    m_nextRealOffset = 0;
}

auto AnnotatedBinary::binary() -> Binary&
{
    return m_binary;
}

auto AnnotatedBinary::binary() const -> const Binary&
{
    return m_binary;
}

void AnnotatedBinary::skip(size_t skipSize)
{
    if (skipSize > 0)
    {
        auto const sectionSize =
            m_binary.size() - m_currentObject->offset() - m_currentObject->size();
        m_currentObject->addSection(m_nextRealOffset, sectionSize);
        m_nextRealOffset = m_currentObject->realSize() + skipSize;
    }
}

void AnnotatedBinary::endObject()
{
    auto const sectionSize =
        m_binary.size() - m_currentObject->offset() - m_currentObject->size();
    if (sectionSize > 0)
    {
        m_currentObject->addSection(m_nextRealOffset, sectionSize);
    }
    m_objects.push_back(std::move(m_currentObject));
}

void AnnotatedBinary::append(AnnotatedBinary&& other)
{
    auto const oldSize = m_binary.size();
    m_binary.append(other.m_binary);
    for (auto&& object : other.m_objects)
    {
        object->changeOffset(object->offset() + oldSize);
        m_objects.push_back(std::move(object));
    }
}

auto AnnotatedBinary::objectCount() const -> size_t
{
    return m_objects.size();
}

void AnnotatedBinary::addUnresolvedReference(const std::shared_ptr<AddressStorageFormat>& format,
                                             const binary::DataPath& destination)
{
    UnresolvedReference reference{m_currentObject->path(), m_currentObject->size()};
    reference.setDestination(destination);
    reference.setFormat(format);
    m_currentObject->addUnresolvedReference(std::move(reference));
}

auto AnnotatedBinary::relativeOffset() const -> size_t
{
    return m_binary.size() - m_currentObject->size();
}

void AnnotatedBinary::enter(const binary::DataPathElement& child)
{
    m_currentPath /= child;
}

void AnnotatedBinary::leave()
{
    m_currentPath.goUp();
}

void AnnotatedBinary::save(const std::filesystem::path& metaFile,
                           const std::filesystem::path& binaryFile) const
{
    m_binary.save(binaryFile);
    m_binaryPath = binaryFile;

    std::ofstream metaOutput{metaFile};
    if (metaOutput.good())
    {
        LuaWriter writer;
        writer.start();
        serialize(writer);
        writer.finish();
        metaOutput << "return " << writer.lua();
    }
    else
    {
        throw FuseException{"could not open meta output file '" + metaFile.string() + "'"};
    }
}

void AnnotatedBinary::serialize(LuaWriter& writer) const
{
    if (m_binaryPath)
    {
        writer.startField("binary").writePath(*m_binaryPath).finishField();
    }
    writer.startField("objects").startTable();
    for (auto const& object : m_objects)
    {
        writer.startField();
        object->serialize(writer);
        writer.finishField();
    }
    writer.finishTable().finishField();
}

auto AnnotatedBinary::object(size_t index) const -> const Object*
{
    Expects(index < objectCount());
    return m_objects[index].get();
}

} // namespace fuse