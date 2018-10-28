#include <fuse/graphics/font/Glyph.h>
#include <algorithm>

namespace fuse::graphics {

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

auto Glyph::characters() const -> const std::string&
{
    return m_characters;
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
    glyph.m_characters = m_characters;
    glyph.m_baseline = m_baseline;

    if (m_shrinkToFit)
    {
        shrinkToFit(glyph);
    }
    else
    {
        extractRegion(glyph);
    }

    return glyph;
}

void GlyphBuilder::validate()
{
    /*
    if (!m_shrinkToFit)
    {
        const bool fitsTile =
            m_region.left() < m_data.width() && m_region.right() <= m_data.width() &&
            m_region.top() < m_data.height() && m_region.bottom() <= m_data.height();
        if (!fitsTile)
        {
            throw std::runtime_error{"region does not fit tile"};
        }
    }
    const bool baseLineFits = m_baseline < m_data.height();
    if (!baseLineFits)
    {
        throw std::runtime_error{"baseline does not fit tile"};
    }
    if (m_characters.empty())
    {
        throw std::runtime_error{"empty characters not allowed in glyph"};
    }
    */
}

void GlyphBuilder::shrinkToFit(Glyph& glyph)
{
    size_t left{m_data.width()};
    size_t right{0};
    size_t top{m_data.height()};
    size_t bottom{0};

    for (size_t y = 0; y < m_data.height(); ++y)
    {
        for (size_t x = 0; x < m_data.width(); ++x)
        {
            if (m_data.pixel(x, y) != m_backgroundColor)
            {
                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }

    /*
    m_region.setLeft(left);
    m_region.setRight(right);
    m_region.setTop(top);
    m_region.setBottom(bottom);
    */
    extractRegion(glyph);
}

void GlyphBuilder::extractRegion(Glyph&)
{
    /*
    glyph.m_tile = Tile{m_region.width(), m_region.height()};
    for (auto y = m_region.top(); y < m_region.bottom(); ++y)
    {
        for (auto x = m_region.left(); x < m_region.right(); ++x)
        {
            auto const pixel = m_data.pixel(x, y);
            glyph.m_tile.setPixel(x - m_region.left(), y - m_region.top(), pixel);
        }
    }
    */
}

} // namespace fuse::graphics