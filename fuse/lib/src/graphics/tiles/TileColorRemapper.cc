#include <fuse/graphics/tiles/TileColorRemapper.h>
#include <diagnostics/Contracts.h>

namespace fuse::graphics {

auto TileColorRemapper::makeGrayScaler(unsigned bitsPerPixel) -> TileColorRemapper
{
    Expects(bitsPerPixel <= 8);
    TileColorRemapper remapper;
    auto const colorCount = 1U << bitsPerPixel;
    for (auto i = 0U; i < colorCount; ++i)
    {
        auto const grayElement = static_cast<uint8_t>(i * (0xFF / colorCount));
        const Tile::pixel_t gray =
            0xFF000000 | grayElement | (grayElement << 8) | (grayElement << 16);
        remapper.addMapping(i, gray);
    }
    return remapper;
}

TileColorRemapper::TileColorRemapper(
    std::initializer_list<std::pair<const Tile::pixel_t, Tile::pixel_t>> remap,
    Tile::pixel_t missingColor)
    : m_colorRemap{remap}
    , m_missingColor{missingColor}
{
}

void TileColorRemapper::setMissingColor(Tile::pixel_t color)
{
    m_missingColor = color;
}

void TileColorRemapper::addMapping(Tile::pixel_t from, Tile::pixel_t to)
{
    m_colorRemap[from] = to;
}

auto TileColorRemapper::transform(const Tile& tile) const -> Tile
{
    Tile newTile{tile.width(), tile.height()};
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