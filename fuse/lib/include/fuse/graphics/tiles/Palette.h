#pragma once

#include <cstdint>
#include <initializer_list>
#include <optional>
#include <vector>

namespace fuse::graphics {

class Palette
{
public:
    using color_t = uint32_t;

    explicit Palette(std::initializer_list<color_t> colors);

    auto count() const -> size_t;
    auto color(size_t index) const -> color_t;
    auto index(color_t color) const -> std::optional<size_t>;

private:
    std::vector<color_t> m_palette;
};

} // namespace fuse::graphics