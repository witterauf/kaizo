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

auto Font::findLongestMatch(const std::string& string, size_t index) const -> std::optional<size_t>
{
    std::optional<size_t> maybeMatch;
    std::string characters;
    size_t length = 0;
    while (index + length < string.length())
    {
        characters += string[index + length];
        if (auto maybeIndex = find(characters))
        {
            maybeMatch = maybeIndex;
            length++;
        }
        else
        {
            break;
        }
    }
    return maybeMatch;
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

} // namespace fuse::graphics