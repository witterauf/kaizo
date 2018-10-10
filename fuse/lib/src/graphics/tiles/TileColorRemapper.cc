#include <fuse/graphics/tiles/TileColorRemapper.h>

namespace fuse::graphics {

TileColorRemapper::TileColorRemapper(
    std::initializer_list<std::pair<const Tile::pixel_t, Tile::pixel_t>> remap,
    Tile::pixel_t missingColor)
    : m_colorRemap{remap}
    , m_missingColor{missingColor}
{
}

auto TileColorRemapper::transform(const Tile& tile) const -> Tile
{
    Tile newTile{ tile.width(), tile.height() };
    for (auto y = 0U; y < tile.height(); ++y)
    {
        for (auto x = 0U; x < tile.width(); ++x)
        {
            auto const color = m_colorRemap.find(tile.pixel(x, y));
            if (color != m_colorRemap.cend())
            {
                newTile.setPixel(x, y, color->second);
            }
            else
            {
                newTile.setPixel(x, y, m_missingColor);
            }
        }
    }
    return newTile;
}

} // namespace fuse::graphics