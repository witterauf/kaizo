#pragma once

#include <cstdint>
#include <initializer_list>
#include <optional>
#include <vector>

namespace kaizo {

class Palette
{
public:
    using color_t = uint32_t;

    explicit Palette(std::initializer_list<color_t> colors);
    explicit Palette(const size_t count);

    auto count() const -> size_t;
    auto color(const size_t index) const -> color_t;
    void setColor(const size_t index, const color_t color);
    auto index(const color_t color) const -> std::optional<size_t>;

private:
    std::vector<color_t> m_palette;
};

} // namespace kaizo