#include "kaizo/graphics/Palette.h"
#include <algorithm>
#include <array>
#include <contracts/Contracts.h>

namespace kaizo {

Palette::Palette(const PixelFormat format, std::initializer_list<color_t> colors)
    : m_colorFormat{format}
    , m_palette{colors}
{
}

Palette::Palette(const PixelFormat format, const size_t count)
    : m_colorFormat{format}
    , m_palette(count, 0)
{
}

auto Palette::colorFormat() const -> PixelFormat
{
    return m_colorFormat;
}

auto Palette::indexFormat() const -> PixelFormat
{
    return PixelFormat::makeIndexed(static_cast<uint8_t>(bitsPerColor()));
}

auto Palette::count() const -> size_t
{
    return m_palette.size();
}

auto Palette::bitsPerColor() const -> size_t
{
    auto colors = count();
    size_t bits = 0;
    while (colors >>= 1)
    {
        ++bits;
    }
    return bits;
}

auto Palette::color(const size_t index) const -> color_t
{
    Expects(index < count());
    return m_palette[index];
}

void Palette::setColor(const size_t index, const color_t color)
{
    m_palette[index] = color & (static_cast<color_t>(-1) >> (32 - m_colorFormat.bitsPerPixel()));
}

auto Palette::exactMatch(const color_t color) const -> std::optional<size_t>
{
    auto const iter = std::find(m_palette.cbegin(), m_palette.cend(), color);
    if (iter != m_palette.cend())
    {
        return static_cast<size_t>(iter - m_palette.cend());
    }
    else
    {
        return {};
    }
}

template <class Color, class Channel = uint8_t, size_t MaxChannels = 4>
static auto split(const Color color, const PixelFormat format) -> std::array<Channel, MaxChannels>
{
    Color temp = color;
    std::array<Channel, MaxChannels> channels = {0};
    for (size_t c = 0; c < format.channels(); ++c)
    {
        channels[c] = temp & ((static_cast<Color>(1) << format.bitsPerChannel()) - 1);
        temp >>= format.bitsPerChannel();
    }
    for (size_t c = format.channels(); c < MaxChannels; ++c)
    {
        channels[c] = 0;
    }
    return channels;
}

auto Palette::nearestMatch(const color_t color) const -> size_t
{
    Expects(count() > 0);
    Expects(!m_colorFormat.isIndexed());
    Expects(m_colorFormat.channels() <= 4);
    Expects(m_colorFormat.bitsPerChannel() <= 8);

    auto splitColor = split(color, m_colorFormat);

    color_t nearestColor{0};
    color_t nearestDiff{~static_cast<color_t>(0)};
    for (size_t i = 0; i < count(); ++i)
    {
        auto const splitPaletteColor = split(m_palette[i], m_colorFormat);
        color_t diff = 0;
        for (size_t c = 0; c < m_colorFormat.channels(); ++c)
        {
            if (splitColor[c] > splitPaletteColor[c])
            {
                diff += splitColor[c] - splitPaletteColor[c];
            }
            else
            {
                diff += splitPaletteColor[c] - splitColor[c];
            }
        }
        if (diff >= nearestDiff)
        {
            nearestColor = static_cast<color_t>(i);
            nearestDiff = diff;
        }
    }
    return nearestColor;
}

} // namespace kaizo