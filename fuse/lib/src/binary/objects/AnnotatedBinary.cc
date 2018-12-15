#include <fstream>
#include <fuse/FuseException.h>
#include <fuse/binary/objects/AnnotatedBinary.h>

using namespace fuse::binary;

namespace fuse {

void AnnotatedBinary::startObject(const binary::DataPath& path)
{
    m_currentPath = path;
    m_currentObject = Object{m_currentPath, m_binary.size()};
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
        auto const sectionSize = m_binary.size() - m_currentObject.size();
        auto const realOffset = m_currentObject.realSize() + skipSize;
        m_currentObject.addSection(m_nextRealOffset, sectionSize);
    }
}

void AnnotatedBinary::endObject()
{
    auto const sectionSize = m_binary.size() - m_currentObject.size();
    auto const realOffset = m_currentObject.realSize();
    m_currentObject.addSection(m_nextRealOffset, sectionSize);
    m_objects.insert(std::make_pair(m_currentPath, std::move(m_currentObject)));
}

void AnnotatedBinary::append(const AnnotatedBinary& other)
{
    auto const oldSize = m_binary.size();
    m_binary.append(other.m_binary);
    for (auto elementPair : other.m_objects)
    {
        elementPair.second.changeOffset(elementPair.second.offset() + oldSize);
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
    UnresolvedReference reference{m_currentPath, relativeOffset()};
    reference.setDestination(destination);
    reference.setFormat(format);
    m_currentObject.addUnresolvedReference(std::move(reference));
}

auto AnnotatedBinary::relativeOffset() const -> size_t
{
    return m_binary.size() - m_currentObject.size();
}

void AnnotatedBinary::enter(const binary::DataPathElement& child)
{
    m_currentPath /= child;
}

void AnnotatedBinary::leave()
{
    m_currentPath.goUp();
}

static void writeObject(std::ofstream& out, const Object& object)
{
    out << "    {\n";
    out << "      offset = " << object.offset() << ",\n";
    out << "      sections = {\n";
    for (auto i = 0U; i < object.sectionCount(); ++i)
    {
        auto const& section = object.section(i);
        out << "        {\n";
        out << "          offset = " << section.offset << ",\n";
        out << "          real_offset = " << section.realOffset << ",\n";
        out << "          size = " << section.size << ",\n";
        out << "        },\n";
    }
    out << "      unresolved_references = {\n";
    for (auto i = 0U; i < object.unresolvedReferenceCount(); ++i)
    {
        auto const& reference = object.unresolvedReference(i);
        out << "        origin_path = \"" << reference.originPath().toString() << "\",\n";
        out << "        referenced_path = \"" << reference.referencedPath().toString() << "\",\n";
        out << "        relative_offset = " << reference.relativeOffset() << ",\n";
        out << "        address_layout = " << reference.addressLayout().asLua() << "\n";
    }
    out << "      },\n";
    out << "    },\n";
}

void AnnotatedBinary::save(const std::filesystem::path& metaFile,
                           const std::filesystem::path& binaryFile)
{
    m_binary.save(binaryFile);

    std::ofstream metaOutput{metaFile};
    if (metaOutput.good())
    {
        metaOutput << "return {\n";
        metaOutput << "  binary = [[" << binaryFile.string() << "]],\n";
        metaOutput << "  objects = {\n";
        for (auto const& objectPair : m_objects)
        {
            writeObject(metaOutput, objectPair.second);
        }
        metaOutput << "  }\n";
        metaOutput << "}\n";
    }
    else
    {
        throw FuseException{"could not open meta output file '" + metaFile.string() + "'"};
    }
}

} // namespace fuse