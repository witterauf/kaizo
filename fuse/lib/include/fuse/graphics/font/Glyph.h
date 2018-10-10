#pragma once

#include <fuse/Rectangle.h>
#include <fuse/graphics/tiles/Tile.h>
#include <string>

namespace fuse::graphics {

class Glyph
{
    friend class GlyphBuilder;

public:
    using pixel_t = Tile::pixel_t;

    auto width() const -> size_t;
    auto height() const -> size_t;
    auto baseline() const -> size_t;
    auto ascent() const -> size_t;
    auto descent() const -> size_t;
    auto advanceWidth() const -> size_t;

    auto characters() const -> const std::string&;
    auto backgroundColor() const -> pixel_t;
    auto bitsPerPixel() const -> size_t;

    auto pixel(size_t x, size_t y) const -> pixel_t;
    auto operator()(size_t x, size_t y) const -> pixel_t;

private:
    Tile m_tile;
    std::string m_characters;
    size_t m_baseline;
    Tile::pixel_t m_background;
    size_t m_bitsPerPixel{1};
    size_t m_advanceWidth;
};

class GlyphBuilder
{
public:
    auto advanceWidth(size_t width) -> GlyphBuilder&;
    auto bitsPerPixel(size_t bpp) -> GlyphBuilder&;
    auto baseline(size_t y) -> GlyphBuilder&;
    auto background(Glyph::pixel_t color) -> GlyphBuilder&;
    auto characters(const std::string& characters) -> GlyphBuilder&;
    auto data(const Tile& tile) -> GlyphBuilder&;
    auto region(const TileRegion& rectangle) -> GlyphBuilder&;
    auto shrinkToFit() -> GlyphBuilder&;
    auto build() -> Glyph;

private:
    void validate();
    void shrinkToFit(Glyph& glyph);
    void extractRegion(Glyph& glyph);

    bool m_shrinkToFit{false};
    TileRegion m_region;
    size_t m_baseline;
    Tile m_data;
    std::string m_characters;
    Glyph::pixel_t m_background;
    size_t m_bitsPerPixel{1};
};

} // namespace fuse::graphics