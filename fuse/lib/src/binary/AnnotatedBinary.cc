#include <fuse/binary/AnnotatedBinary.h>

namespace fuse {

void AnnotatedBinary::startObject(const binary::DataPath& path)
{
    m_currentPath = path;
    m_currentObject = Object{m_binary.size()};
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
        m_currentObject.size = m_binary.size() - m_currentObject.offset;
        Section newSection;
        auto const& lastSection = m_currentObject.sections.back();
        newSection.offset = m_currentObject.size;
        auto const sectionSize = newSection.offset - lastSection.offset;
        newSection.realOffset = lastSection.realOffset + sectionSize + skipSize;
        m_currentObject.sections.push_back(newSection);
    }
}

void AnnotatedBinary::endObject()
{
    m_currentObject.size = m_binary.size() - m_currentObject.offset;
    m_objects.insert(std::make_pair(m_currentPath, std::move(m_currentObject)));
}

void AnnotatedBinary::append(const AnnotatedBinary& other)
{
    auto const oldSize = m_binary.size();
    m_binary.append(other.m_binary);
    for (auto elementPair : other.m_objects)
    {
        elementPair.second.offset += oldSize;
        m_objects.insert(std::move(elementPair));
    }
}

auto AnnotatedBinary::objectCount() const -> size_t
{
    return m_objects.size();
}

auto AnnotatedBinary::unresolvedReferenceCount() const -> size_t
{
    return m_references.size();
}

} // namespace fuse