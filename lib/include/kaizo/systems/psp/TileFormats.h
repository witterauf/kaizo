#pragma once

#include <kaizo/graphics/TileFormat.h>

namespace kaizo {

class Psp4bppTileFormatFactory : public TileFormatFactory
{
public:
    auto make(const std::map<std::string, unsigned>& properties) const
        -> std::unique_ptr<TileFormat> override;
};

class Psp4bppTileFormat : public TileFormat
{
public:
    void setTileSize(const size_t width, const size_t height);
    auto requiredSize(size_t count) const -> size_t override;
    auto write(MutableBinaryView& buffer, offset_t offset, const Tile& tile) -> offset_t override;
    auto read(const BinaryView& buffer, offset_t offset) -> std::pair<Tile, offset_t> override;

private:
    size_t m_width{0};
    size_t m_height{0};
};

} // namespace kaizo