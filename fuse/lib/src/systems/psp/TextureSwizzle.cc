#include <diagnostics/Contracts.h>
#include <fuse/systems/psp/TextureSwizzle.h>

namespace fuse::psp {

static constexpr unsigned BlockWidth = 16;
static constexpr unsigned BlockHeight = 8;
static constexpr size_t BlockSize = BlockHeight * BlockWidth;

auto swizzle(const Binary& unswizzled, unsigned width, size_t start, size_t end) -> Binary
{
    Expects((end - start) % BlockSize == 0);
    Expects(width % BlockWidth == 0);

    Binary swizzled{unswizzled};
    auto const size = end == 0 ? unswizzled.size() - start : end - start;
    auto const blockCount = size / BlockSize;
    auto const blocksPerRow = width / BlockWidth;
    for (auto block = 0U; block < blockCount; ++block)
    {
        auto const by = (block / blocksPerRow) * BlockHeight;
        auto const bx = (block % blocksPerRow) * BlockWidth;
        for (auto y = 0U; y < BlockHeight; ++y)
        {
            for (auto x = 0U; x < BlockWidth; ++x)
            {
                swizzled[start + block * BlockSize + y * BlockWidth + x] =
                    unswizzled[start + (by + y) * width + bx + x];
            }
        }
    }
    return swizzled;
}

auto unswizzle(const Binary& swizzled, unsigned width, size_t start, size_t end) -> Binary
{
    Expects((end - start) % BlockSize == 0);
    Expects(width % BlockWidth == 0);

    end = end == 0 ? swizzled.size() : end;
    Binary unswizzled{swizzled};
    auto const size = end - start;
    auto const blockCount = size / BlockSize;
    auto const blocksPerRow = width / BlockWidth;
    for (auto block = 0U; block < blockCount; ++block)
    {
        auto const by = (block / blocksPerRow) * BlockHeight;
        auto const bx = (block % blocksPerRow) * BlockWidth;
        for (auto y = 0U; y < BlockHeight; ++y)
        {
            for (auto x = 0U; x < BlockWidth; ++x)
            {
                unswizzled[start + (by + y) * width + bx + x] =
                    swizzled[start + block * BlockSize + y * BlockWidth + x];
            }
        }
    }
    return unswizzled;
}

} // namespace fuse::psp
