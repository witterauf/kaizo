#include "kaizo/graphics/Tile.h"
#include <algorithm>
#include <contracts/Contracts.h>

namespace kaizo {

Tile::Tile(const size_t width, const size_t height, const PixelFormat format)
    : m_width{width}
    , m_height{height}
    , m_format{format}
    , m_data(width * height, 0)
{
    Expects(width > 0);
    Expects(height > 0);
    Expects(format.bitsPerPixel() <= 32);
}

auto Tile::width() const -> size_t
{
    return m_width;
}

auto Tile::height() const -> size_t
{
    return m_height;
}

auto Tile::pixelCount() const -> size_t
{
    return m_width * m_height;
}

auto Tile::bitsPerPixel() const -> uint8_t
{
    return static_cast<uint8_t>(m_format.bitsPerPixel());
}

auto Tile::format() const -> PixelFormat
{
    return m_format;
}

void Tile::setPixel(const size_t x, const size_t y, const pixel_t value)
{
    m_data[offset(x, y)] = value & (static_cast<pixel_t>(-1) >> (32 - bitsPerPixel()));
}

void Tile::setPixel(const size_t index, const pixel_t value)
{
    m_data[index] = value & (static_cast<pixel_t>(-1) >> (32 - bitsPerPixel()));
}

auto Tile::pixel(const size_t x, const size_t y) const -> pixel_t
{
    return m_data[offset(x, y)];
}

auto Tile::pixel(const size_t index) const -> pixel_t
{
    return m_data[index];
}

auto Tile::data() const -> const pixel_t*
{
    return m_data.data();
}

auto Tile::dataSize() const -> size_t
{
    return m_data.size();
}

auto Tile::offset(const size_t x, const size_t y) const -> size_t
{
    Expects(x < width());
    Expects(y < height());
    return y * width() + x;
}

auto Tile::crop(const TileRegion& region) const -> Tile
{
    Expects(contains(region));

    Tile clipped{region.width(), region.height(), format()};
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

void Tile::blit(size_t x, size_t y, const Tile& source, const TileRegion& region,
                const pixel_t background)
{
    if (x >= width() || y >= height())
    {
        return;
    }

    auto const xmax = x + region.width() > width() ? width() - x : region.width();
    auto const ymax = y + region.height() > height() ? height() - y : region.height();
    for (size_t ty = 0; ty < ymax; ++ty)
    {
        for (size_t tx = 0; tx < xmax; ++tx)
        {
            if (source.pixel(region.left() + tx, region.top() + ty) != background)
            {
                setPixel(x + tx, y + ty, source.pixel(region.left() + tx, region.top() + ty));
            }
        }
    }
}

auto Tile::boundingBox(const pixel_t background) const -> TileRegion
{
    Expects(bitsPerPixel() == sizeof(pixel_t) || background < (1UL << bitsPerPixel()));

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

} // namespace kaizo