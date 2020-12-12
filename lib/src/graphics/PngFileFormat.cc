#include "PngFileFormat.h"
#include <contracts/Contracts.h>
#include <lodepng.h>

namespace kaizo {

auto PngFileFormat::load(const std::filesystem::path& path)
    -> std::optional<std::pair<Tile, std::optional<Palette>>>
{
    std::vector<unsigned char> data;
    unsigned width, height;
    if (auto error = lodepng::decode(data, width, height, path.string()))
    {
        return {};
    }

    Tile image{width, height, PixelFormat::rgba()};
    for (auto i = 0U; i < image.pixelCount(); ++i)
    {
        image.setPixel(i, reinterpret_cast<const Tile::pixel_t*>(data.data())[i]);
    }
    return std::make_pair(image, std::optional<Palette>{});
}

bool PngFileFormat::save(const std::filesystem::path& path, const Tile& image, const Palette*)
{
    Expects(image.format() == PixelFormat::rgba());

    if (auto error = lodepng_encode32_file(
            path.string().c_str(), reinterpret_cast<const unsigned char*>(image.data()),
            static_cast<unsigned int>(image.width()), static_cast<unsigned int>(image.height())))
    {
        return false;
    }
    return true;
}

} // namespace kaizo