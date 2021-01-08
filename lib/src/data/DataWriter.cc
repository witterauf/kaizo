#include <contracts/Contracts.h>
#include <fuse/binary/DataWriter.h>

namespace kaizo::data {

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

void DataWriter::abortData()
{
}

void DataWriter::finishData()
{
    Expects(m_sectionIndex == 1);
    Expects(m_path == m_root);
    section().annotated.endObject();
    mergeSections();
}

void DataWriter::startNewObject()
{
    section().annotated.endObject();
    section().annotated.startObject(m_path);
}

void DataWriter::startNewObject(const size_t fixedOffset)
{
    startNewObject();
    section().annotated.setFixedOffset(fixedOffset);
}

void DataWriter::mergeSections()
{
    for (auto i = 1U; i < m_sections.size(); ++i)
    {
        m_sections.front().annotated.append(std::move(std::move(m_sections)[i].annotated));
    }
    m_sections.resize(1);
}

auto DataWriter::assemble() -> AnnotatedBinary
{
    return std::move(std::move(m_sections).front()).annotated;
}

auto DataWriter::binary() -> Binary&
{
    return section().annotated.binary();
}

void DataWriter::enter(const DataPathElement& element)
{
    m_path /= element;
    section().annotated.enter(element);
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
    section().annotated.leave();
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

void DataWriter::addUnresolvedReference(const std::shared_ptr<AddressLayout>& format,
                                        const DataPath& destination)
{
    section().annotated.addUnresolvedReference(format, destination);
}

auto DataWriter::path() const -> const DataPath&
{
    return m_path;
}

} // namespace kaizo::data