#pragma once

#include "Tile.h"
#include <vector>

namespace fuse::graphics {

class Image;
class TileImageFormat;
class TileTransformation;

class TileList
{
public:
    static auto fromImage(const Image& image, const TileImageFormat& format) -> TileList;

    auto toImage(const TileImageFormat& format) -> Image;

    void append(const Tile& tile);
    void append(Tile&& tile);

    auto count() const -> size_t;
    auto tile(size_t index) const -> const Tile&;
    auto tile(size_t index) -> Tile&;
    auto operator[](size_t index) const -> const Tile&;
    auto operator[](size_t index) -> Tile&;

    auto transform(const TileTransformation& transformation) -> TileList;

private:
    std::vector<Tile> m_tiles;
};

} // namespace fuse::graphics