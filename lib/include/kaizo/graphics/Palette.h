#pragma once

#include "PixelFormat.h"
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <vector>

namespace kaizo {

class Palette
{
public:
    using color_t = uint32_t;

    Palette(const Palette&) = default;
    auto operator=(const Palette&) -> Palette& = default;
    explicit Palette(const PixelFormat format, std::initializer_list<color_t> colors);
    explicit Palette(const PixelFormat format, const size_t count);

    auto colorFormat() const -> PixelFormat;
    auto indexFormat() const -> PixelFormat;
    auto count() const -> size_t;
    auto bitsPerColor() const -> size_t;
    auto color(const size_t index) const -> color_t;
    void setColor(const size_t index, const color_t color);
    auto exactMatch(const color_t color) const -> std::optional<size_t>;
    auto nearestMatch(const color_t color) const -> size_t;

private:
    PixelFormat m_colorFormat;
    std::vector<color_t> m_palette;
};

} // namespace kaizo