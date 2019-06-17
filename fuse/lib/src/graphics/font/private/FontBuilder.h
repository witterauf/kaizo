#pragma once

#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/graphics/tiles/TileTransformation.h>
#include <map>
#include <memory>

namespace fuse::graphics {

class FontBuilder
{
public:
    struct GlyphDescriptor
    {
        size_t bitmapIndex;
        std::string characters;
        TileRegion boundingBox;
        size_t baseLine;
        bool shrinkToFit{true};
    };

    using FontMetrics = Font::Metrics;

    struct GlyphPixelFormat
    {
        unsigned int bitsPerPixel{1};
        Glyph::pixel_t backgroundColor{0};
    };

    void appendBitmap(const Image& image);
    void setFontMetrics(const FontMetrics& metrics);
    void addGlyph(const GlyphDescriptor& glyph);
    void setPixelTransformation(std::unique_ptr<TileTransformation>&& transformation);
    void setGlyphPixelFormat(const GlyphPixelFormat& format);

    auto build() const -> Font;

    auto fontMetrics() const -> const FontMetrics&;
    auto bitmap() const -> const Image&;
    auto backgroundColor() const -> Glyph::pixel_t;
    auto transformation() const -> const TileTransformation*;

private:
    FontMetrics m_metrics;
    GlyphPixelFormat m_pixelFormat;
    std::unique_ptr<TileTransformation> m_transformation;
    std::vector<GlyphDescriptor> m_glyphs;
    std::vector<Image> m_bitmaps;
};

} // namespace fuse::graphics