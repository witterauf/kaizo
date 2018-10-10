#include <diagnostics/Contracts.h>
#include <fuse/graphics/tiles/Tile.h>

namespace fuse::graphics {

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

} // namespace fuse::graphics