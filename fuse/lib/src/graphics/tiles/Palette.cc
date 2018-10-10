#include <diagnostics/Contracts.h>
#include <fuse/graphics/tiles/Palette.h>

namespace fuse::graphics {

Palette::Palette(std::initializer_list<color_t> colors)
    : m_palette{colors}
{
}

auto Palette::count() const -> size_t
{
    return m_palette.size();
}

auto Palette::color(size_t index) const -> color_t
{
    Expects(index < count());
    return m_palette[index];
}

auto Palette::index(color_t color) const -> std::optional<size_t>
{
    for (auto i = 0U; i < count(); ++i)
    {
        if (m_palette[i] == color)
        {
            return i;
        }
    }
    return {};
}

} // namespace fuse::graphics