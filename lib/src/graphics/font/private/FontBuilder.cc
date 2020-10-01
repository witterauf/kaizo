#include "FontBuilder.h"
#include <diagnostics/Contracts.h>

namespace fuse::graphics {

void FontBuilder::appendBitmap(const Image& image)
{
    m_bitmaps.push_back(image);
}

void FontBuilder::setFontMetrics(const FontMetrics& metrics)
{
    m_metrics = metrics;
}

void FontBuilder::addGlyph(const GlyphDescriptor& glyph)
{
    Expects(glyph.bitmapIndex > 0);
    Expects(glyph.bitmapIndex - 1 < m_bitmaps.size());
    m_glyphs.push_back(glyph);
}

void FontBuilder::setPixelTransformation(std::unique_ptr<TileTransformation>&& transformation)
{
    m_transformation = std::move(transformation);
}

void FontBuilder::setGlyphPixelFormat(const GlyphPixelFormat& format)
{
    m_pixelFormat = format;
}

auto FontBuilder::build() const -> Font
{
    Font font;
    font.setMetrics(m_metrics);
    GlyphBuilder builder;
    builder.background(m_pixelFormat.backgroundColor);
    font.setBackgroundColor(m_pixelFormat.backgroundColor);

    for (auto const& glyph : m_glyphs)
    {
        auto tile = Tile::extractFrom(m_bitmaps[glyph.bitmapIndex - 1], glyph.boundingBox);
        if (m_transformation)
        {
            tile = m_transformation->transform(tile);
        }
        builder.baseline(glyph.baseLine);
        builder.characters(glyph.characters);
        builder.data(tile);
        builder.shrinkToFit(glyph.shrinkToFit);
        font.addGlyph(builder.build());
    }

    return font;
}

auto FontBuilder::fontMetrics() const -> const FontMetrics&
{
    return m_metrics;
}

auto FontBuilder::transformation() const -> const TileTransformation*
{
    return m_transformation.get();
}

auto FontBuilder::backgroundColor() const -> Glyph::pixel_t
{
    return m_pixelFormat.backgroundColor;
}

} // namespace fuse::graphics