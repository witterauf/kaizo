#pragma once

#include "Glyph.h"
#include <fuse/graphics/tiles/Palette.h>
#include <optional>
#include <vector>

namespace fuse::graphics {

class Font
{
public:
    struct Metrics
    {
        size_t lineHeight;
        size_t baseLine;
    };

    void setMetrics(const Metrics& metrics);
    auto lineHeight() const -> size_t;
    auto baseLine() const -> size_t;

    void addGlyph(const Glyph& glyph);
    auto glyphCount() const -> size_t;
    auto glyph(size_t index) const -> const Glyph&;

    auto find(const std::string& characters) const -> std::optional<size_t>;
    auto findLongestMatch(const std::string& string, size_t index) const -> std::optional<size_t>;
    auto toGlyphs(const std::string& string) const -> std::vector<std::optional<size_t>>;

private:
    std::vector<Glyph> m_characters;
    size_t m_baseline;
    size_t m_lineHeight;
    unsigned int m_bitsPerPixel{1};
    size_t m_backgroundColor{0};
};

} // namespace fuse::graphics