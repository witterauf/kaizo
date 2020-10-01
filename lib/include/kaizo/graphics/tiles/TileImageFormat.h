#pragma once

#include <fuse/Rectangle.h>
#include <cstddef>

namespace fuse::graphics {

class TileImageFormat
{
    friend class TileImageFormatBuilder;

public:
    using TileRectangle = Rectangle<size_t>;

    bool isValid() const;
    auto tile(size_t x, size_t y) const -> TileRectangle;
    auto tile(size_t index) const -> TileRectangle;

    auto imageWidth() const -> size_t;
    auto imageHeight() const -> size_t;
    auto tileCount() const -> size_t;
    auto tileCountX() const -> size_t;
    auto tileCountY() const -> size_t;

private:
    bool m_isValid{false};
    size_t m_imageWidth{0};
    size_t m_imageHeight{0};
    size_t m_tileWidth{0};
    size_t m_tileHeight{0};
    size_t m_tileXCount{0};
    size_t m_tileYCount{0};
    unsigned int m_borderLeft{0};
    unsigned int m_borderTop{0};
    unsigned int m_borderRight{0};
    unsigned int m_borderBottom{0};
    unsigned int m_gridHorizontal{0};
    unsigned int m_gridVertical{0};
};

class TileImageFormatBuilder
{
public:
    auto setBorder(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
        -> TileImageFormatBuilder&;
    auto setGrid(unsigned int horizontal, unsigned int vertical) -> TileImageFormatBuilder&;
    auto setImageSize(size_t width, size_t height) -> TileImageFormatBuilder&;
    auto setTileCount(size_t x, size_t y) -> TileImageFormatBuilder&;
    auto build() -> TileImageFormat;

private:
    TileImageFormat m_format;
};

} // namespace fuse::graphics