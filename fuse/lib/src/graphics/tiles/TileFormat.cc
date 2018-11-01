#include <diagnostics/Contracts.h>
#include <fuse/Binary.h>
#include <fuse/graphics/tiles/Tile.h>
#include <fuse/graphics/tiles/TileFormat.h>
#include <fuse/graphics/tiles/TileList.h>

namespace fuse::graphics {

auto TileFormat::make(const std::string& name, const std::map<std::string, unsigned>& properties)
    -> std::unique_ptr<TileFormat>
{
    if (name == "psp-4bpp")
    {
        return Psp4bppFormat::make(properties);
    }
    else
    {
        return {};
    }
}

auto TileFormat::writeList(Binary& buffer, offset_t offset, const TileList& list) -> offset_t
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

auto PlanarTileFormat::write(Binary& buffer, offset_t offset, const Tile& tile) -> offset_t
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
                buffer.size();
                buffer[byteOffset] &= ~(0x80 >> x);
                buffer[byteOffset] |= set ? (0x80 >> x) : 0;
                offset++;
            }
        }
    }
    return offset;
}

auto PlanarTileFormat::read(const Binary&, offset_t) -> std::pair<Tile, offset_t>
{
    return {{}, 0};
}

auto Psp4bppFormat::make(const std::map<std::string, unsigned>& properties)
    -> std::unique_ptr<TileFormat>
{
    auto format = std::make_unique<Psp4bppFormat>();
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

void Psp4bppFormat::setTileSize(size_t width, size_t height)
{
    m_width = width;
    m_height = height;
}

auto Psp4bppFormat::requiredSize(size_t count) const -> size_t
{
    return count * m_width * m_height / 2;
}

auto Psp4bppFormat::write(Binary& buffer, offset_t offset, const Tile& tile) -> offset_t
{
    Expects(offset % 8 == 0);
    for (auto y = 0U; y < m_height; ++y)
    {
        for (auto x = 0U; x < m_width; x += 2)
        {
            auto const byteOffset = offset / 8;
            buffer[byteOffset] = (tile.pixel(x, y) & 0x0F) | ((tile.pixel(x, y) & 0x0F) << 4);
            offset += 8;
        }
    }
    return offset;
}

auto Psp4bppFormat::read(const Binary& buffer, offset_t offset) -> std::pair<Tile, offset_t>
{
    Expects(offset % 8 == 0);
    Tile tile{m_width, m_height};
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

} // namespace fuse::graphics