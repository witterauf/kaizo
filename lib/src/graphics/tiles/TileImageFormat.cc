#include <diagnostics/Contracts.h>
#include <fuse/graphics/tiles/TileImageFormat.h>

namespace fuse::graphics {

auto TileImageFormatBuilder::setBorder(unsigned int left, unsigned int top, unsigned int right,
                                       unsigned int bottom) -> TileImageFormatBuilder&
{
    m_format.m_borderLeft = left;
    m_format.m_borderTop = top;
    m_format.m_borderRight = right;
    m_format.m_borderBottom = bottom;
    return *this;
}

auto TileImageFormatBuilder::setGrid(unsigned int horizontal, unsigned int vertical)
    -> TileImageFormatBuilder&
{
    m_format.m_gridHorizontal = horizontal;
    m_format.m_gridVertical = vertical;
    return *this;
}

auto TileImageFormatBuilder::setImageSize(size_t width, size_t height) -> TileImageFormatBuilder&
{
    m_format.m_imageWidth = width;
    m_format.m_imageHeight = height;
    return *this;
}

auto TileImageFormatBuilder::setTileCount(size_t x, size_t y) -> TileImageFormatBuilder&
{
    m_format.m_tileXCount = x;
    m_format.m_tileYCount = y;
    return *this;
}

auto TileImageFormatBuilder::build() -> TileImageFormat
{
    if (m_format.m_tileXCount == 0 || m_format.m_tileYCount == 0)
    {
        return {};
    }
    if (m_format.m_borderLeft + m_format.m_borderRight > m_format.m_imageWidth)
    {
        return {};
    }
    auto const internalWidth =
        m_format.m_imageWidth - m_format.m_borderLeft - m_format.m_borderRight;

    auto const horizontalGrid = (m_format.m_tileXCount - 1) * m_format.m_gridHorizontal;
    if (horizontalGrid > internalWidth)
    {
        return {};
    }

    if (m_format.m_borderTop + m_format.m_borderTop > m_format.m_imageHeight)
    {
        return {};
    }
    auto const internalHeight =
        m_format.m_imageHeight - m_format.m_borderTop - m_format.m_borderBottom;

    auto const verticalGrid = (m_format.m_tileYCount - 1) * m_format.m_gridVertical;
    if (verticalGrid > internalHeight)
    {
        return {};
    }

    auto const rawWidth = internalWidth - horizontalGrid;
    auto const rawHeight = internalHeight - verticalGrid;

    m_format.m_tileWidth = rawWidth / m_format.m_tileXCount;
    m_format.m_tileHeight = rawHeight / m_format.m_tileYCount;

    if (m_format.m_tileWidth * m_format.m_tileXCount != rawWidth)
    {
        return {};
    }
    if (m_format.m_tileHeight * m_format.m_tileYCount != rawHeight)
    {
        return {};
    }

    m_format.m_isValid = true;
    return std::move(m_format);
}

bool TileImageFormat::isValid() const
{
    return m_isValid;
}

auto TileImageFormat::tile(size_t x, size_t y) const -> TileRectangle
{
    Expects(isValid());
    Expects(x < tileCountX());
    Expects(y < tileCountY());
    auto const left = m_borderLeft + x * (m_tileWidth + m_gridHorizontal);
    auto const top = m_borderTop + y * (m_tileHeight + m_gridVertical);
    return TileRectangle(left, top, m_tileWidth, m_tileHeight);
}

auto TileImageFormat::tile(size_t index) const -> TileRectangle
{
    Expects(index < tileCount());
    return tile(index % tileCountX(), index / tileCountX());
}

auto TileImageFormat::imageWidth() const -> size_t
{
    return m_imageWidth;
}

auto TileImageFormat::imageHeight() const -> size_t
{
    return m_imageHeight;
}

auto TileImageFormat::tileCount() const -> size_t
{
    return tileCountX() * tileCountY();
}

auto TileImageFormat::tileCountX() const -> size_t
{
    return m_tileXCount;
}

auto TileImageFormat::tileCountY() const -> size_t
{
    return m_tileYCount;
}

} // namespace fuse::graphics