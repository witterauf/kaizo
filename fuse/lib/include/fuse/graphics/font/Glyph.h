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

    auto pixel(size_t x, size_t y) const -> pixel_t;
    auto operator()(size_t x, size_t y) const -> pixel_t;

private:
    Tile m_tile;
    std::string m_characters;
    size_t m_baseline;
    Tile::pixel_t m_background;
};

class GlyphBuilder
{
public:
    auto baseline(size_t y) -> GlyphBuilder&;
    auto background(Glyph::pixel_t color) -> GlyphBuilder&;
    auto characters(const std::string& characters) -> GlyphBuilder&;
    auto data(const Tile& tile) -> GlyphBuilder&;
    auto shrinkToFit(bool shrink = true) -> GlyphBuilder&;
    auto build() -> Glyph;

private:
    void validate();
    void shrink();

    bool m_shrinkToFit{false};
    size_t m_baseline;
    Tile m_data;
    std::string m_characters;
    Glyph::pixel_t m_backgroundColor{0};
};

} // namespace fuse::graphics