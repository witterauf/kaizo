#pragma once

#include <cstddef>

namespace fuse::link {

class SegmentEncoding
{
public:
    SegmentEncoding() = default;
    explicit SegmentEncoding(size_t width, size_t position);

    bool isValid() const;
    auto segment(size_t address) const -> size_t;
    auto offset(size_t address) const -> size_t;
    auto address(size_t segment) const -> size_t;
    auto segmentSize() const -> size_t;

private:
    size_t m_width = 0;
    size_t m_position = 0;
};

} // namespace fuse::link