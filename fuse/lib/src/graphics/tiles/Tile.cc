#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/tiles/Tile.h>

namespace fuse::graphics {

auto Tile::extractFrom(const Image& image, const TileRegion& region) -> Tile
{
    Expects(image.contains(region));
    Tile extracted{region.width(), region.height()};
    for (auto y = 0U; y < region.height(); ++y)
    {
        for (auto x = 0U; x < region.width(); ++x)
        {
            auto const color = image.pixel(region.left() + x, region.top() + y);
            extracted.setPixel(x, y, color);
        }
    }
    return extracted;
}

Tile::Tile(size_t width, size_t height)
    : m_width{width}
    , m_height{height}
    , m_data(width * height, 0)
{
    Expects(width > 0);
    Expects(height > 0);
}

auto Tile::width() const -> size_t
{
    return m_width;
}

auto Tile::height() const -> size_t
{
    return m_height;
}

void Tile::setPixel(size_t x, size_t y, pixel_t value)
{
    m_data[offset(x, y)] = value;
}

auto Tile::pixel(size_t x, size_t y) const -> pixel_t
{
    return m_data[offset(x, y)];
}

auto Tile::operator()(size_t x, size_t y) -> pixel_t&
{
    return m_data[offset(x, y)];
}

auto Tile::operator()(size_t x, size_t y) const -> pixel_t
{
    return m_data[offset(x, y)];
}

auto Tile::offset(size_t x, size_t y) const -> size_t
{
    Expects(x < width());
    Expects(y < height());
    return y * width() + x;
}

auto Tile::clip(const TileRegion& region) const -> Tile
{
    Expects(contains(region));

    Tile clipped{region.width(), region.height()};
    for (auto y = 0U; y < clipped.height(); ++y)
    {
        for (auto x = 0U; x < clipped.width(); ++x)
        {
            auto const color = pixel(region.left() + x, region.top() + y);
            clipped.setPixel(x, y, color);
        }
    }
    return clipped;
}

auto Tile::boundingBox(pixel_t background) const -> TileRegion
{
    size_t left{m_width};
    size_t right{0};
    size_t top{m_height};
    size_t bottom{0};

    for (size_t y = 0; y < m_height; ++y)
    {
        for (size_t x = 0; x < m_width; ++x)
        {
            if (pixel(x, y) != background)
            {
                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }

    if (right >= left && bottom >= top)
    {
        return TileRegion{left, top, right - left + 1, bottom - top + 1};
    }
    else
    {
        return TileRegion{};
    }
}

bool Tile::contains(const TileRegion& region) const
{
    return region.left() < m_width && region.right() <= m_width && region.top() < m_height &&
           region.bottom() <= m_height;
}

} // namespace fuse::graphics