#pragma once

#include <cstdint>
#include <utility>
#include <vector>

namespace fuse::graphics {

class Tile;
class TileList;

class TileFormat
{
public:
    using offset_t = size_t;

    virtual auto requiredSize(size_t count) const -> size_t = 0;
    virtual auto write(std::vector<uint8_t>& buffer, offset_t offset, const Tile& tile)
        -> offset_t = 0;
    virtual auto read(const std::vector<uint8_t>& buffer, offset_t offset)
        -> std::pair<Tile, offset_t> = 0;

    auto writeList(std::vector<uint8_t>& buffer, offset_t offset, const TileList& list) -> offset_t;
    auto readList(const std::vector<uint8_t>& buffer, offset_t offset, size_t count)
        -> std::pair<TileList, offset_t>;
};

class PlanarTileFormat : public TileFormat
{
public:
    void setTileSize(size_t width, size_t height);
    void setBitsPerPixel(uint8_t bpp);

    auto requiredSize(size_t count) const -> size_t override;
    auto write(std::vector<uint8_t>& buffer, offset_t offset, const Tile& tile)
        -> offset_t override;
    auto read(const std::vector<uint8_t>& buffer, offset_t offset)
        -> std::pair<Tile, offset_t> override;

private:
    size_t m_tileWidth{0};
    size_t m_tileHeight{0};
    uint8_t m_bitsPerPixel{0};
};

class Psp4bppFormat : public TileFormat
{
public:
    void setTileSize(size_t width, size_t height);

    auto requiredSize(size_t count) const -> size_t override;
    auto write(std::vector<uint8_t>& buffer, offset_t offset, const Tile& tile)
        -> offset_t override;
    auto read(const std::vector<uint8_t>& buffer, offset_t offset)
        -> std::pair<Tile, offset_t> override;

private:
    size_t m_width{0}, m_height{0};
};

} // namespace fuse::graphics