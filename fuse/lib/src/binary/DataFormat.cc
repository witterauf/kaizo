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

auto DataFormat::decode(DataReader& reader) -> std::unique_ptr<Data>
{
    auto const unalignedOffset = reader.offset();
    if (unalignedOffset % m_alignment != 0)
    {
        reader.setOffset(unalignedOffset + (m_alignment - (unalignedOffset % m_alignment)));
    }
    return doDecode(reader);
}

void DataFormat::copyTo(DataFormat& format) const
{
    format.m_alignment = m_alignment;
    format.m_storeAs = m_storeAs;
}

} // namespace fuse::binary