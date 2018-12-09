#include <diagnostics/Contracts.h>
#include <fuse/binary/DataWriter.h>

namespace fuse::binary {

DataWriter::DataWriter()
    : m_sections(1)
    , m_sectionIndex{1}
{
}

void DataWriter::startData(const DataPath& root)
{
    Expects(m_sectionIndex == 1);
    m_path = m_root = root;
    section().annotated.startObject(m_path);
}

void DataWriter::finishData()
{
    Expects(m_sectionIndex == 1);
    Expects(m_path == m_root);
    section().annotated.endObject();
}

auto DataWriter::assemble() const -> AnnotatedBinary
{
    AnnotatedBinary overall;
    for (auto i = 0U; i < m_sections.size(); ++i)
    {
        overall.append(m_sections[i].annotated);
    }
    return std::move(overall);
}

auto DataWriter::binary() -> Binary&
{
    return section().annotated.binary();
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
    }
    m_sectionIndex++;

    // section().referencePath = m_path.parent();
    // section().referenceOffset = sectionOffset(-1);
    // section().dataOffsets[m_path].start = sectionOffset();
}

void DataWriter::skip(size_t size)
{
    section().annotated.skip(size);
}

void DataWriter::leaveLevel()
{
    Expects(m_sectionIndex > 0);
    // section().dataSegments.back().end = sectionOffset();
    // m_sectionIndex--;
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

void DataWriter::addUnresolvedReference(const std::shared_ptr<ReferenceFormat>&)
{
    /*
    auto const& path = section().referencePath;
    auto const offset = section().referenceOffset;

    UnresolvedReference reference{path, sectionOffset(-1) - offset};
    reference.setDestination(m_path);
    reference.setFormat(format);
    section().unresolvedReferences.push_back(std::move(reference));
    */
}

} // namespace fuse::binary