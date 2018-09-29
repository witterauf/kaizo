#include <fuse/link/SegmentEncoding.h>

namespace fuse::link {

SegmentEncoding::SegmentEncoding(size_t width, size_t position)
    : m_width{width}
    , m_position{position}
{
}

bool SegmentEncoding::isValid() const
{
    return m_width != 0;
}

auto SegmentEncoding::offset(size_t address) const -> size_t
{
    auto const mask = (1ULL << m_position) - 1;
    return address & mask;
}

auto SegmentEncoding::segment(size_t address) const -> size_t
{
    auto const mask = (1ULL << m_width) - 1;
    return (address >> m_position) & mask;
}

auto SegmentEncoding::address(size_t segment) const -> size_t
{
    return segment << m_position;
}

} // namespace fuse::link