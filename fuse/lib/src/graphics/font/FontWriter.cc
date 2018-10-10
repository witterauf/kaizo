#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/font/Glyph.h>
#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/font/FontWriter.h>

namespace fuse::graphics {

void FontWriter::setFont(const Font* font)
{
    Expects(font);
    m_font = font;
}

void FontWriter::setAlignment(Alignment alignment)
{
    m_alignment = alignment;
}

void FontWriter::write(const std::string& text, Image& image)
{
    Expects(m_font);
    auto const glyphs = m_font->toGlyphs(text);

    size_t ix{0};
    const size_t iy{m_font->baseLine()};
    for (auto const& glyphIndex : glyphs)
    {
        if (glyphIndex)
        {
            auto const& glyph = m_font->glyph(*glyphIndex);
            for (auto y = 0U; y < glyph.height(); ++y)
            {
                for (auto x = 0U; x < glyph.width(); ++x)
                {
                    auto const pixel = glyph.pixel(x, y);
                    image.setPixel(ix + x, iy + glyph.ascent() + y, pixel);
                }
            }
            ix += glyph.advanceWidth();
        }
    }
}

auto FontWriter::width(const std::string& text) -> size_t
{
    Expects(m_font);
    auto const glyphs = m_font->toGlyphs(text);
    size_t width{0};
    for (auto const& glyph : glyphs)
    {
        if (glyph)
        {
            width += m_font->glyph(*glyph).advanceWidth();
        }
    }
    return width;
}

auto FontWriter::height(const std::string& text) -> size_t
{
    Expects(m_font);
    auto const glyphs = m_font->toGlyphs(text);
    size_t height{0};
    for (auto const& glyph : glyphs)
    {
        if (glyph)
        {
            height = std::max(height, m_font->glyph(*glyph).height());
        }
    }
    return height;
}

} // namespace fuse::graphics