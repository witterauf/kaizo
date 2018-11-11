#include <diagnostics/Contracts.h>
#include <fuse/graphics/font/Font.h>

namespace fuse::graphics {

void Font::addGlyph(const Glyph& glyph)
{
    m_characters.push_back(glyph);
}

auto Font::glyphCount() const -> size_t
{
    return m_characters.size();
}

auto Font::glyph(size_t index) const -> const Glyph&
{
    Expects(index < glyphCount());
    return m_characters[index];
}

auto Font::find(const std::string& characters) const -> std::optional<size_t>
{
    for (auto i = 0U; i < glyphCount(); ++i)
    {
        if (glyph(i).characters() == characters)
        {
            return i;
        }
    }
    return {};
}

auto Font::findStartingWith(const std::string& characters) const -> std::vector<size_t>
{
    std::vector<size_t> matchingGlyphs;
    for (auto i = 0U; i < glyphCount(); ++i)
    {
        if (glyph(i).characters().substr(0, characters.size()) == characters)
        {
            matchingGlyphs.push_back(i);
        }
    }
    return matchingGlyphs;
}

auto Font::findLongestMatch(const std::string& string, size_t index) const -> std::optional<size_t>
{
    std::optional<size_t> longestMatch;
    size_t matchSize{0};
    for (auto glyphIndex = 0U; glyphIndex < glyphCount(); ++glyphIndex)
    {
        auto const characters = glyph(glyphIndex).characters();
        auto i = 0U;
        for (; i < characters.size(); ++i)
        {
            if (characters[i] != string[index + i])
            {
                break;
            }
        }
        if (i == characters.size() && i > matchSize)
        {
            longestMatch = glyphIndex;
            matchSize = i;
        }
    }
    return longestMatch;
}

auto Font::toGlyphs(const std::string& string) const -> std::vector<std::optional<size_t>>
{
    size_t index{0};
    std::vector<std::optional<size_t>> glyphs;
    while (index < string.length())
    {
        auto maybeMatch = findLongestMatch(string, index);
        glyphs.push_back(maybeMatch);
        if (maybeMatch)
        {
            index += glyph(*maybeMatch).characters().length();
        }
        else
        {
            index += 1;
        }
    }
    return glyphs;
}

void Font::setMetrics(const Metrics& metrics)
{
    Expects(metrics.baseLine < metrics.lineHeight);
    m_metrics = metrics;
}

auto Font::lineHeight() const -> size_t
{
    return m_metrics.lineHeight;
}

auto Font::baseLine() const -> size_t
{
    return m_metrics.baseLine;
}

auto Font::backgroundColor() const -> Glyph::pixel_t
{
    return m_backgroundColor;
}

void Font::setBackgroundColor(Glyph::pixel_t color)
{
    m_backgroundColor = color;
}

} // namespace fuse::graphics