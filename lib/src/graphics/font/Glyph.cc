#include <kaizo/graphics/font/Glyph.h>

namespace kaizo {

auto Glyph::width() const -> size_t
{
    return m_tile.width();
}

auto Glyph::height() const -> size_t
{
    return m_tile.height();
}

auto Glyph::baseline() const -> size_t
{
    return m_baseline;
}

auto Glyph::ascent() const -> size_t
{
    return m_baseline;
}

auto Glyph::descent() const -> size_t
{
    return height() - baseline();
}

auto Glyph::advanceWidth() const -> size_t
{
    return width() + 1;
}

auto Glyph::characters() const -> const std::string&
{
    return m_characters;
}

auto Glyph::boundingBox() const -> TileRegion
{
    return m_tile.boundingBox(m_background);
}

auto Glyph::backgroundColor() const -> pixel_t
{
    return m_background;
}

auto Glyph::pixel(size_t x, size_t y) const -> pixel_t
{
    return m_tile.pixel(x, y);
}

auto Glyph::operator()(size_t x, size_t y) const -> pixel_t
{
    return pixel(x, y);
}

auto GlyphBuilder::baseline(size_t y) -> GlyphBuilder&
{
    m_baseline = y;
    return *this;
}

auto GlyphBuilder::background(Glyph::pixel_t color) -> GlyphBuilder&
{
    m_backgroundColor = color;
    return *this;
}

auto GlyphBuilder::characters(const std::string& characters) -> GlyphBuilder&
{
    m_characters = characters;
    return *this;
}

auto GlyphBuilder::data(const Tile& tile) -> GlyphBuilder&
{
    m_data = tile;
    return *this;
}

auto GlyphBuilder::shrinkToFit(bool shrink) -> GlyphBuilder&
{
    m_shrinkToFit = shrink;
    return *this;
}

auto GlyphBuilder::build() -> Glyph
{
    validate();

    Glyph glyph;
    if (m_shrinkToFit)
    {
        shrink();
    }
    glyph.m_tile = m_data;
    glyph.m_characters = m_characters;
    glyph.m_baseline = m_baseline;
    glyph.m_background = m_backgroundColor;
    return glyph;
}

void GlyphBuilder::validate()
{
    if (m_baseline >= m_data.height())
    {
        throw std::runtime_error{"baseline does not fit tile"};
    }
    if (m_characters.empty())
    {
        throw std::runtime_error{"empty characters not allowed in glyph"};
    }
}

void GlyphBuilder::shrink()
{
    auto const boundingBox = m_data.boundingBox(m_backgroundColor);
    if (boundingBox.hasArea())
    {
        if (m_baseline < boundingBox.top() || m_baseline >= boundingBox.bottom())
        {
            throw std::runtime_error{"GlyphBuilder: baseline is not within bounding box"};
        }
        m_baseline -= boundingBox.top();
        m_data = m_data.clip(boundingBox);
    }
    else
    {
        throw std::runtime_error{"GlyphBuilder: shrunk region is empty for '" + m_characters + "'"};
    }
}

} // namespace kaizo