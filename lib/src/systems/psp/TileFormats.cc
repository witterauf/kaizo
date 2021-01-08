#include "kaizo/systems/psp/TileFormats.h"
#include "kaizo/graphics/Tile.h"
#include <contracts/Contracts.h>

namespace kaizo {

auto Psp4bppTileFormatFactory::make(const std::map<std::string, unsigned>& properties) const
    -> std::unique_ptr<TileFormat>
{
    auto format = std::make_unique<Psp4bppTileFormat>();
    if (properties.cend() != properties.find("width") &&
        properties.cend() != properties.find("height"))
    {
        format->setTileSize(properties.at("width"), properties.at("height"));
        return std::move(format);
    }
    else
    {
        throw std::runtime_error{"Psp4bppTileFormat: required properties missing"};
    }
}

void Psp4bppTileFormat::setTileSize(const size_t width, const size_t height)
{
    m_width = width;
    m_height = height;
}

auto Psp4bppTileFormat::requiredSize(size_t count) const -> size_t
{
    return count * m_width * m_height / 2;
}

auto Psp4bppTileFormat::write(MutableBinaryView& buffer, offset_t offset, const Tile& tile)
    -> offset_t
{
    Expects(offset % 8 == 0);
    for (auto y = 0U; y < m_height; ++y)
    {
        for (auto x = 0U; x < m_width; x += 2)
        {
            auto const byteOffset = offset / 8;
            buffer[byteOffset] = (tile.pixel(x, y) & 0x0F) | ((tile.pixel(x + 1, y) & 0x0F) << 4);
            offset += 8;
        }
    }
    return offset;
}

auto Psp4bppTileFormat::read(const BinaryView& buffer, offset_t offset) -> std::pair<Tile, offset_t>
{
    Expects(offset % 8 == 0);
    Tile tile{m_width, m_height, PixelFormat::makeIndexed(4)};
    for (auto y = 0U; y < m_height; ++y)
    {
        for (auto x = 0U; x < m_width; x += 2)
        {
            auto const byteOffset = offset / 8;
            tile.setPixel(x, y, buffer[byteOffset] & 0x0F);
            tile.setPixel(x + 1, y, buffer[byteOffset] >> 4);
            offset += 8;
        }
    }
    return std::make_pair(tile, offset);
}

} // namespace kaizo