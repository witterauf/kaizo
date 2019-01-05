#include <fstream>
#include <fuse/FuseException.h>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <fuse/lua/LuaWriter.h>

using namespace fuse::binary;

namespace fuse {

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
        auto const sectionSize = m_binary.size() - m_currentObject->size();
        m_currentObject->addSection(m_nextRealOffset, sectionSize);
        m_nextRealOffset = m_currentObject->realSize() + skipSize;
    }
}

void AnnotatedBinary::endObject()
{
    auto const sectionSize = m_binary.size() - m_currentObject->size();
    if (sectionSize > 0)
    {
        m_currentObject->addSection(m_nextRealOffset, sectionSize);
    }
    m_objects.insert(std::make_pair(m_currentPath, std::move(m_currentObject)));
}

void AnnotatedBinary::append(AnnotatedBinary&& other)
{
    auto const oldSize = m_binary.size();
    m_binary.append(other.m_binary);
    for (auto&& elementPair : other.m_objects)
    {
        elementPair.second->changeOffset(elementPair.second->offset() + oldSize);
        m_objects.insert(std::move(elementPair));
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
    for (auto const& objectPair : m_objects)
    {
        writer.startField();
        objectPair.second->serialize(writer);
        writer.finishField();
    }
    writer.finishTable().finishField();
}

} // namespace fuse