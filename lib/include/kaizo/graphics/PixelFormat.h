#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace kaizo {

class PixelFormat
{
public:
    enum class Format : uint8_t
    {
        RGBA = 0,
        Indexed = 1,
    };

    static auto makeIndexed(const uint8_t bitsPerPixel) -> PixelFormat;
    static auto makeChannels(const Format format, const uint8_t bitsPerPixel,
                             const uint8_t bitsPerChannel) -> PixelFormat;
    static auto fromString(const std::string& format) -> std::optional<PixelFormat>;

    static auto rgba() -> PixelFormat;

    auto bitsPerPixel() const -> unsigned;
    auto channels() const -> unsigned;
    auto bitsPerChannel() const -> unsigned;
    auto format() const -> Format;

    bool operator==(const PixelFormat rhs) const;

private:
    explicit PixelFormat(const Format format, const uint8_t channels, const uint8_t bitsPerChannel,
                         const uint8_t bitsPerPixel);

    // [ 7: 0] bits per pixel
    // [15: 8] bits per channel
    // [23:16] channels
    // [31:24] format
    const uint32_t m_format;
};

} // namespace kaizo