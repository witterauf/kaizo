#include <diagnostics/Contracts.h>
#include <fuse/binary/DataWriter.h>

namespace fuse::binary {

auto DataWriter::binary() -> Binary&
{
    return section().binaries.back().binary;
}

void DataWriter::enter(const DataPathElement& element)
{
    m_path /= element;
}

void DataWriter::enterLevel()
{
    if (m_sectionIndex >= m_sections.size())
    {
        m_sections.push_back({});
        m_sections.back().binaries.push_back({});
    }
    m_sectionIndex++;

    section().referencePath = m_path.parent();
    section().referenceOffset = sectionOffset(-1);
    section().dataOffsets[m_path] = sectionOffset();
}

auto DataWriter::sectionOffset(int relative) const -> size_t
{
    return section(relative).binaries.back().offset +
           section(relative).binaries.back().binary.size();
}

void DataWriter::skip(size_t size)
{
    section().binaries.push_back({});
    section().binaries.back().offset = sectionOffset() + size;
}

void DataWriter::leaveLevel()
{
    Expects(m_sectionIndex > 0);
    m_sectionIndex--;
}

void DataWriter::leave()
{
    m_path.goUp();
}

auto DataWriter::section(int relative) -> Section&
{
    Expects(m_sectionIndex <= m_sections.size() - relative);
    return m_sections[m_sectionIndex - relative - 1];
}

auto DataWriter::section(int relative) const -> const Section&
{
    Expects(m_sectionIndex <= m_sections.size() - relative);
    return m_sections[m_sectionIndex - relative - 1];
}

void DataWriter::addUnresolvedReference(const std::shared_ptr<ReferenceFormat>& format)
{
    auto const& path = section().referencePath;
    auto const offset = section().referenceOffset;

    UnresolvedReference reference{path, sectionOffset(-1) - offset};
    reference.setDestination(m_path);
    reference.setFormat(format);
    section().unresolvedReferences.push_back(std::move(reference));
}

} // namespace fuse::binary