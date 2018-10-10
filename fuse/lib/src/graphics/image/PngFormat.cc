#include "PngFormat.h"
#include "lodepng.h"

namespace fuse::graphics {

auto PngFormat::load(const std::filesystem::path& path) -> std::optional<Image>
{
    std::vector<unsigned char> data;
    unsigned width, height;
    if (auto error = lodepng::decode(data, width, height, path.string()))
    {
        return {};
    }

    Image image{width, height, Image::PixelFormat::RGBA, 4 * 8};
    for (auto i = 0U; i < image.pixelCount(); ++i)
    {
        image[i] = reinterpret_cast<const Image::pixel_t*>(data.data())[i];
    }
    return image;
}

bool PngFormat::save(const std::filesystem::path& path, const Image& image)
{
    if (auto error = lodepng_encode32_file(
            path.string().c_str(), reinterpret_cast<const unsigned char*>(image.data()),
            static_cast<unsigned int>(image.width()), static_cast<unsigned int>(image.height())))
    {
        return false;
    }
    return true;
}

} // namespace fuse::graphics