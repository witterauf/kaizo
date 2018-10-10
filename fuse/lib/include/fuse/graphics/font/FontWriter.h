#pragma once

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

    void setFont(const Font* font);
    void setAlignment(Alignment alignment);

    void setCharacterSpacing(CharacterSpacing spacing);
    void setMonospace(size_t characterWidth);
    void setProportional();

    void write(const std::string& text, Image& image);
    auto width(const std::string& text) -> size_t;
    auto height(const std::string& text) -> size_t;

private:
    Alignment m_alignment{Alignment::Left};
    CharacterSpacing m_characterSpacing{CharacterSpacing::Proportional};
    const Font* m_font{nullptr};
};

} // namespace fuse::graphics