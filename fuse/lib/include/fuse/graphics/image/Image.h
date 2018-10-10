#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

namespace fuse::graphics {

class Image
{
public:
    using pixel_t = uint32_t;

    enum class PixelFormat
    {
        RGBA,
        RGB,
        Indexed
    };

    static auto load(const std::filesystem::path& path) -> std::optional<Image>;

    Image() = default;
    explicit Image(size_t width, size_t height, PixelFormat format, uint8_t bitsPerPixel);

    auto width() const -> size_t;
    auto height() const -> size_t;
    auto pixelCount() const -> size_t;

    auto data() const -> const pixel_t*;
    void setPixel(size_t x, size_t y, pixel_t value);
    auto pixel(size_t x, size_t y) const -> pixel_t;
    auto operator()(size_t x, size_t y) -> pixel_t&;
    auto operator()(size_t x, size_t y) const -> pixel_t;
    auto operator[](size_t index) const -> pixel_t;
    auto operator[](size_t index) -> pixel_t&;

private:
    auto offset(size_t x, size_t y) const -> size_t;

    PixelFormat m_format;
    uint8_t m_bitsPerPixel{0};

    size_t m_width{0};
    size_t m_height{0};
    std::vector<pixel_t> m_data;
};

} // namespace fuse::graphics