#include <diagnostics/Contracts.h>
#include <fstream>
#include <fuse/FuseException.h>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>
#include <fuse/utilities/DomReaderHelpers.h>

using namespace fuse::binary;

namespace fuse {

auto AnnotatedBinary::deserialize(LuaDomReader& reader) -> std::unique_ptr<AnnotatedBinary>
{
    Expects(reader.isRecord());
    auto* binary = new AnnotatedBinary;

    auto const binaryFileName = requireString(reader, "binary");
    binary->m_binary = Binary::load(binaryFileName);

    enterArray(reader, "objects");
    auto const size = reader.size();
    for (auto i = 0U; i < size; ++i)
    {
        reader.enter(i);
        auto object = PackedObject::deserialize(reader, binary);
        binary->m_objects.push_back(std::move(object));
        reader.leave();
    }
    reader.leave();
    return std::unique_ptr<AnnotatedBinary>(binary);
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
    auto const sectionSize = m_binary.size() - m_currentObject->offset() - m_currentObject->size();
    if (sectionSize > 0)
    {
        m_currentObject->addSection(m_nextRealOffset, sectionSize);
    }
    if (m_currentObject->size() > 0)
    {
        m_objects.push_back(std::move(m_currentObject));
    }
    else
    {
        m_currentObject = nullptr;
    }
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
    UnresolvedReference reference{m_currentObject->path(),
                                  m_binary.size() - m_currentObject->offset()};
    reference.setDestination(destination);
    reference.setFormat(format);
    m_currentObject->addUnresolvedReference(std::move(reference));
}

void AnnotatedBinary::setFixedOffset(const size_t offset)
{
    m_currentObject->setFixedOffset(offset);
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

auto AnnotatedBinary::object(size_t index) -> Object*
{
    Expects(index < objectCount());
    return m_objects[index].get();
}

} // namespace fuse