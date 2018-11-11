#include "FontBuilder.h"

namespace fuse::graphics {

void FontBuilder::setFontMetrics(const FontMetrics& metrics)
{
    m_metrics = metrics;
}

void FontBuilder::addGlyph(const GlyphDescriptor& glyph)
{
    m_glyphs.push_back(glyph);
}

void FontBuilder::setPixelTransformation(std::unique_ptr<TileTransformation>&& transformation)
{
    m_transformation = std::move(transformation);
}

void FontBuilder::setBitmap(const Image& image)
{
    m_bitmap = image;
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

    for (auto const& glyph : m_glyphs)
    {
        auto tile = Tile::extractFrom(m_bitmap, glyph.boundingBox);
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

auto FontBuilder::bitmap() const -> const Image&
{
    return m_bitmap;
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