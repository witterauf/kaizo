#pragma once

#include "Binary.h"
#include "BinaryView.h"

namespace fuse {

class BinaryPatch
{
public:
    static constexpr size_t MaximumSize = 8;

    explicit BinaryPatch(uint64_t data, uint64_t mask, size_t size, ptrdiff_t relativeOffset = 0);
    explicit BinaryPatch(const uint8_t* data, const uint8_t* mask, size_t size,
                         ptrdiff_t relativeOffset = 0);
    explicit BinaryPatch(const Binary& data, int64_t relativeOffset = 0);
    void apply(MutableBinaryView& binary, size_t offset) const;

    auto data() const -> const uint8_t*;
    bool usesOnlyFullBytes() const;
    auto size() const -> size_t;
    void setRelativeOffset(ptrdiff_t offset);
    auto relativeOffset() const -> ptrdiff_t;

private:
    uint8_t m_data[MaximumSize];
    uint8_t m_mask[MaximumSize];
    ptrdiff_t m_offset;
    size_t m_size;
};

} // namespace fuse