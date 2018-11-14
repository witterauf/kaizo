#include <diagnostics/Contracts.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/DataFormat.h>
#include <fuse/binary/DataReader.h>

namespace fuse::binary {

void DataFormat::storeAs(const std::string& label)
{
    m_storeAs = label;
}

void DataFormat::doNotStore()
{
    m_storeAs = {};
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
    auto const unalignedOffset = reader.offset() + m_skipBefore;
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

void DataFormat::copyDataFormat(DataFormat& format) const
{
    format.m_skipAfter = m_skipAfter;
    format.m_skipBefore = m_skipBefore;
    format.m_alignment = m_alignment;
    format.m_storeAs = m_storeAs;
}

} // namespace fuse::binary