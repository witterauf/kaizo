#pragma once

#include "Tile.h"
#include "TileTransformation.h"
#include <initializer_list>
#include <map>

namespace fuse::graphics {

class TileColorRemapper : public TileTransformation
{
public:
    static auto makeGrayScaler(unsigned bitsPerPixel) -> TileColorRemapper;

    TileColorRemapper() = default;
    explicit TileColorRemapper(
        std::initializer_list<std::pair<const Tile::pixel_t, Tile::pixel_t>> remap,
        Tile::pixel_t missingColor = 0);

    void setMissingColor(Tile::pixel_t color);
    void addMapping(Tile::pixel_t from, Tile::pixel_t to);
    auto transform(const Tile& tile) const -> Tile override;

private:
    Tile::pixel_t m_missingColor{0};
    std::map<Tile::pixel_t, Tile::pixel_t> m_colorRemap;
};

} // namespace fuse::graphics