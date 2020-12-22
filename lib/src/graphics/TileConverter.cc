#include "kaizo/graphics/TileConverter.h"
#include "kaizo/graphics/Tile.h"
#include <contracts/Contracts.h>

namespace kaizo {

TileConverter::TileConverter(const PixelFormat from, const PixelFormat to)
    : m_fromFormat{from}
    , m_toFormat{to}
{
}

auto TileConverter::fromFormat() const -> PixelFormat
{
    return m_fromFormat;
}

auto TileConverter::toFormat() const -> PixelFormat
{
    return m_toFormat;
}

Palettizer::Palettizer(const Palette& palette)
    : TileConverter{palette.colorFormat(), palette.indexFormat()}
    , m_palette{palette}
{
}

void Palettizer::setPalette(const Palette& palette)
{
    m_palette = palette;
    setFromFormat(palette.colorFormat());
    setToFormat(palette.indexFormat());
}

auto Palettizer::convert(const Tile& tile) const -> Tile
{
    Expects(tile.format() == fromFormat());
    Tile result(tile.width(), tile.height(), toFormat());
    for (size_t y = 0; y < tile.height(); ++y)
    {
        for (size_t x = 0; x < tile.width(); ++x)
        {
            result.setPixel(x, y,
                            static_cast<Tile::pixel_t>(m_palette.nearestMatch(tile.pixel(x, y))));
        }
    }
    return result;
}

} // namespace kaizo