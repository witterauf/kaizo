#include <diagnostics/Contracts.h>
#include <fuse/BinaryPatch.h>

namespace fuse {

BinaryPatch::BinaryPatch(uint64_t data, uint64_t mask, size_t size, ptrdiff_t relativeOffset)
    : m_offset{relativeOffset}
{
    Expects(size >= 0 && size < MaximumSize);
    for (auto i = 0U; i < MaximumSize; ++i)
    {
        m_data[i] = data & 0xFF;
        m_mask[i] = mask & 0xFF;
        data >>= 8;
        mask >>= 8;
    }
    m_size = size;
}

BinaryPatch::BinaryPatch(const Binary& data, int64_t relativeOffset)
    : m_offset{relativeOffset}
{
    Expects(data.size() > 0 && data.size() <= MaximumSize);
    for (auto i = 0U; i < MaximumSize; ++i)
    {
        if (i < data.size())
        {
            m_data[i] = data[i];
            m_mask[i] = 0xFF;
        }
        else
        {
            m_data[i] = m_mask[i] = 0;
        }
    }
    m_size = data.size();
}

auto BinaryPatch::data() const -> const uint8_t*
{
    return m_data;
}

void BinaryPatch::apply(MutableBinaryView& binary, size_t offset) const
{
    for (auto i = 0U; i < size(); ++i)
    {
        auto const reverseMask = ~m_mask[i];
        binary[offset + m_offset + i] =
            (binary[offset + m_offset + i] & reverseMask) | (m_data[i] & m_mask[i]);
    }
}

auto BinaryPatch::size() const -> size_t
{
    return m_size;
}

void BinaryPatch::setRelativeOffset(ptrdiff_t offset)
{
    m_offset = offset;
}

auto BinaryPatch::relativeOffset() const -> ptrdiff_t
{
    return m_offset;
}

bool BinaryPatch::usesOnlyFullBytes() const
{
    for (auto i = 0U; i < size(); ++i)
    {
        if (m_mask[i] != 0xFF)
        {
            return false;
        }
    }
    return true;
}

} // namespace fuse