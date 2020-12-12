#include "kaizo/graphics/PixelFormat.h"
#include <algorithm>
#include <map>
#include <stdexcept>

namespace {

auto toLower(std::string string) -> std::string
{
    std::transform(string.begin(), string.end(), string.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return std::move(string);
}

} // namespace

namespace kaizo {

auto PixelFormat::makeIndexed(const uint8_t bitsPerPixel) -> PixelFormat
{
    return PixelFormat{Format::Indexed, 1, bitsPerPixel, bitsPerPixel};
}

auto PixelFormat::makeChannels(const Format format, const uint8_t bitsPerPixel,
                               const uint8_t bitsPerChannel) -> PixelFormat
{
    switch (format)
    {
    case Format::RGBA: return PixelFormat{format, 4, bitsPerChannel, bitsPerPixel};
    default: throw std::runtime_error{"invalid pixel format"};
    }
}

static const std::map<std::string, PixelFormat> FormatNames = {
    {"rgba", PixelFormat::rgba()},
    {"indexed8", PixelFormat::makeIndexed(8)},
    {"indexed4", PixelFormat::makeIndexed(4)}};

auto PixelFormat::fromString(const std::string& format) -> std::optional<PixelFormat>
{
    auto const formatLower = toLower(format);
    auto const iter = FormatNames.find(formatLower);
    if (iter != FormatNames.cend())
    {
        return iter->second;
    }
    else
    {
        return {};
    }
}

auto PixelFormat::rgba() -> PixelFormat
{
    return makeChannels(Format::RGBA, 32, 8);
}

auto PixelFormat::bitsPerPixel() const -> unsigned
{
    return m_format & 0xff;
}

auto PixelFormat::channels() const -> unsigned
{
    return (m_format >> 16) & 0xff;
}

auto PixelFormat::bitsPerChannel() const -> unsigned
{
    return (m_format >> 8) & 0xff;
}

auto PixelFormat::format() const -> Format
{
    return static_cast<Format>(m_format >> 24);
}

bool PixelFormat::operator==(const PixelFormat rhs) const
{
    return m_format == rhs.m_format;
}

PixelFormat::PixelFormat(const Format format, const uint8_t channels, const uint8_t bitsPerChannel,
                         const uint8_t bitsPerPixel)
    : m_format{(static_cast<uint8_t>(format) << 24) | (channels << 16) | (bitsPerChannel << 8) |
               static_cast<uint32_t>(bitsPerPixel)}
{
}

} // namespace kaizo