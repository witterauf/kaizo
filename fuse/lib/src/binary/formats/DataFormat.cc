#include <diagnostics/Contracts.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/formats/DataFormat.h>

namespace fuse::binary {

void DataFormat::storeAs(const std::string& label)
{
    m_storeAs = label;
}

void DataFormat::doNotStore()
{
    m_storeAs = {};
}

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
    writer.skip(m_skipBefore);
    doEncode(writer, data);
    writer.skip(m_skipAfter);
}

void DataFormat::copyDataFormat(DataFormat& format) const
{
    format.m_offset = m_offset;
    format.m_skipAfter = m_skipAfter;
    format.m_skipBefore = m_skipBefore;
    format.m_alignment = m_alignment;
    format.m_storeAs = m_storeAs;
}

void DataFormat::track(DataReader& reader, size_t offset, size_t size)
{
    if (m_trackTag)
    {
        reader.trackRange(*m_trackTag, offset, size);
    }
}

} // namespace fuse::binary