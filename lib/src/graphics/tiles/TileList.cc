#include <diagnostics/Contracts.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/tiles/TileImageFormat.h>
#include <fuse/graphics/tiles/TileList.h>
#include <fuse/graphics/tiles/TileTransformation.h>

namespace fuse::graphics {

auto TileList::fromImage(const Image& image, const TileImageFormat& format) -> TileList
{
    Expects(format.isValid());
    Expects(format.imageWidth() == image.width());
    Expects(format.imageHeight() == image.height());

    TileList list;
    for (auto i = 0U; i < format.tileCount(); ++i)
    {
        auto const rectangle = format.tile(i);
        Tile tile{rectangle.width(), rectangle.height()};
        for (auto y = 0U; y < tile.height(); ++y)
        {
            for (auto x = 0U; x < tile.width(); ++x)
            {
                auto const imagePixel = image.pixel(rectangle.left() + x, rectangle.top() + y);
                tile.setPixel(x, y, imagePixel);
            }
        }
        list.m_tiles.push_back(std::move(tile));
    }
    return list;
}

auto TileList::toImage(const TileImageFormat& format) -> Image
{
    Expects(format.isValid());
    Expects(format.tileCount() >= count());

    Image image{format.imageWidth(), format.imageHeight(), Image::PixelFormat::RGBA, 32};
    for (auto i = 0U; i < count(); ++i)
    {
        auto const rectangle = format.tile(i);
        for (auto y = 0U; y < rectangle.height(); ++y)
        {
            for (auto x = 0U; x < rectangle.width(); ++x)
            {
                auto tilePixel = tile(i).pixel(x, y);
                image.setPixel(rectangle.left() + x, rectangle.top() + y, tilePixel);
            }
        }
    }
    return image;
}

void TileList::append(const Tile& tile)
{
    m_tiles.push_back(tile);
}

void TileList::append(Tile&& tile)
{
    m_tiles.push_back(std::move(tile));
}

auto TileList::count() const -> size_t
{
    return m_tiles.size();
}

auto TileList::tile(size_t index) const -> const Tile&
{
    Expects(index < count());
    return m_tiles[index];
}

auto TileList::tile(size_t index) -> Tile&
{
    Expects(index < count());
    return m_tiles[index];
}

auto TileList::operator[](size_t index) const -> const Tile&
{
    Expects(index < count());
    return m_tiles[index];
}

auto TileList::operator[](size_t index) -> Tile&
{
    Expects(index < count());
    return m_tiles[index];
}

auto TileList::transform(const TileTransformation& transformation) -> TileList
{
    TileList newList;
    for (auto const& tile : m_tiles)
    {
        newList.append(transformation.transform(tile));
    }
    return newList;
}

} // namespace fuse::graphics