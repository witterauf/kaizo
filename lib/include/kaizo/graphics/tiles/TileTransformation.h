#pragma once

namespace fuse::graphics {

class Tile;

class TileTransformation
{
public:
    virtual auto transform(const Tile& tile) const -> Tile = 0;
};

} // namespace fuse::graphics