#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/image/LuaImageLibrary.h>
#include <fuse/graphics/tiles/Tile.h>
#include <fuse/lua/Utilities.h>
#include <sol.hpp>

namespace fuse::graphics {

auto loadImage(const std::string& filename) -> Image
{
    if (auto maybeImage = Image::load(filename))
    {
        return *maybeImage;
    }
    else
    {
        throw std::runtime_error{"could not load image '" + filename + "'"};
    }
}

auto Image_new(const sol::table& arguments) -> Image
{
    auto width = requireField<unsigned>(arguments, "width");
    auto height = requireField<unsigned>(arguments, "height");
    return Image{width, height, Image::PixelFormat::RGBA, 32};
}

auto Image_toTile(const Image& image) -> Tile
{
    return Tile::extractFrom(image, TileRegion{0, 0, image.width(), image.height()});
}

auto makeImage(const Tile& tile) -> Image
{
    return Image{tile, Image::PixelFormat::RGBA, 32};
}

void saveImage(const Image& image, const std::string& filename)
{
    image.save(filename);
}

auto openImageLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);

    sol::table module = lua.create_table();
    module.new_usertype<Image>("Image", "load", sol::factories(&loadImage), "new",
                               sol::factories(&Image_new), "from_tile", sol::factories(makeImage),
                               "save", &saveImage, "getpixel", &Image::pixel, "setpixel",
                               &Image::setPixel, "width", &Image::width, "height", &Image::height,
                               "totile", &Image_toTile);

    return module;
}

} // namespace fuse::graphics