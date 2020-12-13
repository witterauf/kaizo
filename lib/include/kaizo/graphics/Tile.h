#pragma once

#include <cstddef>
#include <cstdint>
#include <fuse/Rectangle.h>
#include <kaizo/graphics/PixelFormat.h>
#include <vector>

namespace kaizo {

using TileRegion = fuse::Rectangle<size_t>;

class Tile
{
public:
    using pixel_t = uint32_t;

    Tile() = default;
    Tile(const Tile& other) = default;
    Tile(Tile&& other) = default;
    explicit Tile(const size_t width, const size_t height, const PixelFormat format);

    auto width() const -> size_t;
    auto height() const -> size_t;
    auto bitsPerPixel() const -> uint8_t;
    auto format() const -> PixelFormat;
    auto pixelCount() const -> size_t;

    bool contains(const TileRegion& region) const;
    auto crop(const TileRegion& region) const -> Tile;
    void blit(const Tile& source, const size_t x, const size_t y, const pixel_t background);
    auto boundingBox(const pixel_t background) const -> TileRegion;

    void setPixel(const size_t x, const size_t y, const pixel_t value);
    void setPixel(const size_t index, const pixel_t value);
    auto pixel(const size_t x, const size_t y) const -> pixel_t;
    auto pixel(const size_t index) const -> pixel_t;
    auto data() const -> const pixel_t*;
    auto dataSize() const -> size_t;

private:
    auto offset(const size_t x, const size_t y) const -> size_t;

    const size_t m_width{0};
    const size_t m_height{0};
    const PixelFormat m_format{PixelFormat::rgba()};
    std::vector<pixel_t> m_data;
};

} // namespace kaizo