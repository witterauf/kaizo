#include <diagnostics/Contracts.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/image/ImageFormat.h>
#include <fuse/graphics/tiles/Tile.h>

namespace fuse::graphics {

auto Image::load(const std::filesystem::path& path) -> std::optional<Image>
{
    auto extension = path.extension().string();
    if (extension.empty())
    {
        return {};
    }
    if (auto format = ImageFormat::makeFormat(extension.substr(1)))
    {
        return format->load(path);
    }
    return {};
}

void Image::save(const std::filesystem::path& path) const
{
    auto extension = path.extension().string();
    if (extension.empty())
    {
        return;
    }
    if (auto format = ImageFormat::makeFormat(extension.substr(1)))
    {
        format->save(path, *this);
    }
}

Image::Image(size_t width, size_t height, PixelFormat format, uint8_t bitsPerPixel)
    : m_format{format}
    , m_bitsPerPixel{bitsPerPixel}
    , m_width{width}
    , m_height{height}
    , m_data(width * height, 0)
{
    Expects(width > 0);
    Expects(height > 0);
    Expects(bitsPerPixel > 0);
}

Image::Image(const Tile& tile, PixelFormat format, uint8_t bitsPerPixel)
    : m_format{format}
    , m_bitsPerPixel{bitsPerPixel}
    , m_width{tile.width()}
    , m_height{tile.height()}
    , m_data(tile.width() * tile.height())
{
    Expects(m_width > 0);
    Expects(m_height > 0);
    Expects(m_bitsPerPixel > 0);

    for (auto y = 0U; y < m_height; ++y)
    {
        for (auto x = 0U; x < m_width; ++x)
        {
            setPixel(x, y, tile.pixel(x, y));
        }
    }
}

auto Image::width() const -> size_t
{
    return m_width;
}

auto Image::height() const -> size_t
{
    return m_height;
}

auto Image::pixelCount() const -> size_t
{
    return width() * height();
}

void Image::setPixel(size_t x, size_t y, pixel_t value)
{
    m_data[offset(x, y)] = value;
}

auto Image::pixel(size_t x, size_t y) const -> pixel_t
{
    return m_data[offset(x, y)];
}

auto Image::operator()(size_t x, size_t y) -> pixel_t&
{
    return m_data[offset(x, y)];
}

auto Image::operator()(size_t x, size_t y) const -> pixel_t
{
    return m_data[offset(x, y)];
}

auto Image::operator[](size_t index) const -> pixel_t
{
    Expects(index < pixelCount());
    return m_data[index];
}

auto Image::operator[](size_t index) -> pixel_t&
{
    Expects(index < pixelCount());
    return m_data[index];
}

auto Image::offset(size_t x, size_t y) const -> size_t
{
    Expects(x < width());
    Expects(y < height());
    return y * width() + x;
}

auto Image::data() const -> const pixel_t*
{
    return m_data.data();
}

} // namespace fuse::graphics