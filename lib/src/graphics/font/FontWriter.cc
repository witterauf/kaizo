#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/font/FontWriter.h>
#include <fuse/graphics/font/Glyph.h>
#include <fuse/graphics/image/Image.h>

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

void FontWriter::write(const std::string& text, Image& image, size_t x, size_t y, Anchor anchor) const
{
    Expects(m_font);
    auto const glyphs = m_font->toGlyphs(text);

    // vertical anchoring
    switch (anchor)
    {
    case Anchor::BottomLeft:
    case Anchor::BottomRight: y -= m_font->lineHeight();
    case Anchor::TopLeft:
    case Anchor::TopRight: y += m_font->baseLine(); break;
    default: break;
    }

    // horizontal anchoring
    switch (anchor)
    {
    case Anchor::BaselineRight:
    case Anchor::TopRight:
    case Anchor::BottomRight: break;
    default: break;
    }

    for (auto const& glyphIndex : glyphs)
    {
        if (glyphIndex)
        {
            auto const& glyph = m_font->glyph(*glyphIndex);
            for (auto gy = 0U; gy < glyph.height(); ++gy)
            {
                for (auto gx = 0U; gx < glyph.width(); ++gx)
                {
                    auto const pixel = glyph.pixel(gx, gy);
                    if (pixel != m_font->backgroundColor())
                    {
                        image.setPixel(x + gx, y - glyph.ascent() + gy, glyph(gx, gy));
                    }
                }
            }
            x += glyph.advanceWidth();
        }
    }
}

auto FontWriter::boundingBox(const std::string& text) -> BitmapRegion
{
    Expects(m_font);
    BitmapRegion boundingBox;
    auto const glyphs = m_font->toGlyphs(text);
    for (auto const& glyph : glyphs)
    {
        if (glyph)
        {
            boundingBox.setRight(boundingBox.right() + m_font->glyph(*glyph).advanceWidth());
            boundingBox.setBottom(std::max(boundingBox.bottom(), m_font->glyph(*glyph).height()));
        }
    }
    return boundingBox;
}

auto FontWriter::width(const std::string& text) -> size_t
{
    Expects(m_font);
    return boundingBox(text).width();
}

auto FontWriter::height(const std::string& text) -> size_t
{
    Expects(m_font);
    return boundingBox(text).height();
}

} // namespace fuse::graphics