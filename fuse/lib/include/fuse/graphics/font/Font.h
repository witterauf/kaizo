#pragma once

#include "Glyph.h"
#include <optional>
#include <vector>

namespace fuse::graphics {

class Font
{
public:
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
};

} // namespace fuse::graphics