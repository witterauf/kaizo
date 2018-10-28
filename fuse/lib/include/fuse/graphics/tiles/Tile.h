#pragma once

#include <cstddef>
#include <cstdint>
#include <fuse/Rectangle.h>
#include <vector>

namespace fuse::graphics {

using TileRegion = Rectangle<size_t>;
class Image;

class Tile
{
public:
    static auto extractFrom(const Image& image, const TileRegion& region) -> Tile;

    using pixel_t = uint32_t;

    Tile() = default;
    explicit Tile(size_t width, size_t height);

    auto width() const -> size_t;
    auto height() const -> size_t;

    void setPixel(size_t x, size_t y, pixel_t value);
    auto pixel(size_t x, size_t y) const -> pixel_t;
    auto operator()(size_t x, size_t y) -> pixel_t&;
    auto operator()(size_t x, size_t y) const -> pixel_t;

private:
    auto offset(size_t x, size_t y) const -> size_t;

    size_t m_width{0};
    size_t m_height{0};
    std::vector<pixel_t> m_data;
};

} // namespace fuse::graphics