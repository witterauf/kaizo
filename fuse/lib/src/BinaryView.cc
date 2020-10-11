#include "fuse/BinaryView.h"

namespace fuse {

BinaryView::BinaryView(const Binary& binary)
    : m_buffer{binary.data()}
    , m_size{binary.size()}
{
}

BinaryView::BinaryView(const uint8_t* buffer, const size_t size)
    : m_buffer{buffer}
    , m_size{size}
{
}

auto BinaryView::data() const -> const uint8_t*
{
    return m_buffer;
}

auto BinaryView::size() const -> size_t
{
    return m_size;
}

auto BinaryView::begin() const -> const uint8_t*
{
    return m_buffer;
}

auto BinaryView::end() const -> const uint8_t*
{
    return m_buffer + m_size;
}

auto BinaryView::operator[](const size_t offset) const -> uint8_t
{
    return m_buffer[offset];
}

} // namespace fuse