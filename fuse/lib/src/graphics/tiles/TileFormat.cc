#include <diagnostics/Contracts.h>
#include <fuse/graphics/tiles/Tile.h>
#include <fuse/graphics/tiles/TileFormat.h>
#include <fuse/graphics/tiles/TileList.h>

namespace fuse::graphics {

auto TileFormat::writeList(std::vector<uint8_t>& buffer, offset_t offset, const TileList& list)
    -> offset_t
{
    for (auto i = 0U; i < list.count(); ++i)
    {
        offset = write(buffer, offset, list.tile(i));
    }
    return offset;
}

void PlanarTileFormat::setTileSize(size_t width, size_t height)
{
    Expects(width > 0);
    Expects(height > 0);
    m_tileWidth = width;
    m_tileHeight = height;
}

void PlanarTileFormat::setBitsPerPixel(uint8_t bpp)
{
    Expects(bpp > 0);
    m_bitsPerPixel = bpp;
}

auto PlanarTileFormat::requiredSize(size_t count) const -> size_t
{
    return count * m_tileWidth * m_tileHeight * m_bitsPerPixel;
}

auto PlanarTileFormat::write(std::vector<uint8_t>& buffer, offset_t offset, const Tile& tile)
    -> offset_t
{
    for (auto y = 0U; y < m_tileHeight; ++y)
    {
        for (auto plane = 0U; plane < m_bitsPerPixel; ++plane)
        {
            for (auto x = 0U; x < m_tileWidth; ++x)
            {
                auto const byteOffset = offset / 8;
                auto const bitOffset = offset % 8;
                auto const pixel = tile.pixel(x, y);
                const bool set = (pixel & (1 << plane)) != 0;
                buffer[byteOffset] &= ~(0x80 >> x);
                buffer[byteOffset] |= set ? (0x80 >> x) : 0;
                offset++;
            }
        }
    }
    return offset;
}

auto PlanarTileFormat::read(const std::vector<uint8_t>&, offset_t)
    -> std::pair<Tile, offset_t>
{
    return { {}, 0 };
}

} // namespace fuse::graphics