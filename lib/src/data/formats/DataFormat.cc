#include <contracts/Contracts.h>
#include <kaizo/data/DataReader.h>
#include <kaizo/data/DataWriter.h>
#include <kaizo/data/data/Data.h>
#include <kaizo/data/formats/DataFormat.h>

namespace kaizo::data {

void DataFormat::setFixedOffset(size_t offset)
{
    m_offset = offset;
}

void DataFormat::setAlignment(size_t alignment)
{
    Expects(alignment > 0);
    m_alignment = alignment;
}

void DataFormat::setSkipAfter(size_t size)
{
    m_skipAfter = size;
}

void DataFormat::setSkipBefore(size_t size)
{
    m_skipBefore = size;
}

auto DataFormat::decode(DataReader& reader) -> std::unique_ptr<Data>
{
    auto const unalignedOffset = m_skipBefore + (m_offset ? *m_offset : reader.offset());
    if (unalignedOffset % m_alignment != 0)
    {
        reader.setOffset(unalignedOffset + (m_alignment - (unalignedOffset % m_alignment)));
    }
    else
    {
        reader.setOffset(unalignedOffset);
    }
    auto data = doDecode(reader);
    reader.setOffset(reader.offset() + m_skipAfter);
    return std::move(data);
}

void DataFormat::encode(DataWriter& writer, const Data& data)
{
    if (m_offset)
    {
        writer.startNewObject(*m_offset);
    }
    // TODO: alignment
    writer.skip(m_skipBefore);
    doEncode(writer, data);
    writer.skip(m_skipAfter);
}

void DataFormat::track(DataReader& reader, size_t offset, size_t size)
{
    if (m_trackTag)
    {
        reader.trackRange(*m_trackTag, offset, size);
    }
}

bool DataFormat::hasTag() const
{
    return m_trackTag.has_value();
}

void DataFormat::setTag(const std::string& tag)
{
    m_trackTag = tag;
}

auto DataFormat::tag() const -> const std::string&
{
    Expects(hasTag());
    return *m_trackTag;
}

} // namespace kaizo::data