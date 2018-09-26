#pragma once

#include <cstddef>

namespace fuse::link {
    
class SegmentEncoding
{
public:
    SegmentEncoding() = default;
    explicit SegmentEncoding(size_t width, size_t position);

    bool isValid() const;
    auto toSegment(size_t address) const -> size_t;
    auto toAddress(size_t segment) const -> size_t;

private:
    size_t m_width = 0;
    size_t m_position = 0;
};
    
}