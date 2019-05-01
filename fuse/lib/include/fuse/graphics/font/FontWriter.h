#pragma once

#include <fuse/Rectangle.h>
#include <string>
#include <vector>

namespace fuse::graphics {

class Image;
class Font;

class FontWriter
{
public:
    enum class Alignment
    {
        Left,
        Center,
        Right
    };

    enum class CharacterSpacing
    {
        Monospace,
        Proportional
    };

    enum class Anchor
    {
        TopLeft,
        BaselineLeft,
        BottomLeft,
        TopRight,
        BaselineRight,
        BottomRight,
    };

    void setFont(const Font* font);
    void setAlignment(Alignment alignment);

    void setCharacterSpacing(CharacterSpacing spacing);
    void setMonospace(size_t characterWidth);
    void setProportional();

    void write(const std::string& text, Image& image, size_t x, size_t y,
               Anchor anchor = Anchor::BaselineLeft) const;
    auto width(const std::string& text) -> size_t;
    auto height(const std::string& text) -> size_t;
    auto boundingBox(const std::string& text) -> BitmapRegion;

private:
    Alignment m_alignment{Alignment::Left};
    CharacterSpacing m_characterSpacing{CharacterSpacing::Proportional};
    const Font* m_font{nullptr};
};

} // namespace fuse::graphics