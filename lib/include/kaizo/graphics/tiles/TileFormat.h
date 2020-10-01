#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace fuse {
class Binary;
}

namespace fuse::graphics {

class Tile;
class TileList;

class TileFormat
{
public:
    using offset_t = size_t;

    static auto make(const std::string& name, const std::map<std::string, unsigned>& properties)
        -> std::unique_ptr<TileFormat>;

    virtual auto requiredSize(size_t count) const -> size_t = 0;
    virtual auto write(Binary& buffer, offset_t offset, const Tile& tile) -> offset_t = 0;
    virtual auto read(const Binary& buffer, offset_t offset) -> std::pair<Tile, offset_t> = 0;

    auto writeList(fuse::Binary& binary, offset_t offset, const TileList& list) -> offset_t;
    auto readList(const fuse::Binary& buffer, offset_t offset, size_t count)
        -> std::pair<TileList, offset_t>;
};

class PlanarTileFormat : public TileFormat
{
public:
    void setTileSize(size_t width, size_t height);
    void setBitsPerPixel(uint8_t bpp);

    auto requiredSize(size_t count) const -> size_t override;
    auto write(fuse::Binary& buffer, offset_t offset, const Tile& tile) -> offset_t override;
    auto read(const fuse::Binary& buffer, offset_t offset) -> std::pair<Tile, offset_t> override;

private:
    size_t m_tileWidth{0};
    size_t m_tileHeight{0};
    uint8_t m_bitsPerPixel{0};
};

class Psp4bppFormat : public TileFormat
{
public:
    static auto make(const std::map<std::string, unsigned>& properties)
        -> std::unique_ptr<TileFormat>;

    void setTileSize(size_t width, size_t height);

    auto requiredSize(size_t count) const -> size_t override;
    auto write(fuse::Binary& buffer, offset_t offset, const Tile& tile) -> offset_t override;
    auto read(const fuse::Binary& buffer, offset_t offset) -> std::pair<Tile, offset_t> override;

private:
    size_t m_width{0}, m_height{0};
};

} // namespace fuse::graphics