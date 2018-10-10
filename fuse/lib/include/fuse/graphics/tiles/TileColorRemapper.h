#pragma once

#include "Tile.h"
#include "TileTransformation.h"
#include <initializer_list>
#include <map>

namespace fuse::graphics {

class TileColorRemapper : public TileTransformation
{
public:
    explicit TileColorRemapper(
        std::initializer_list<std::pair<const Tile::pixel_t, Tile::pixel_t>> remap,
        Tile::pixel_t missingColor = 0);

    auto transform(const Tile& tile) const -> Tile override;

private:
    Tile::pixel_t m_missingColor{0};
    std::map<Tile::pixel_t, Tile::pixel_t> m_colorRemap;
};

} // namespace fuse::graphics