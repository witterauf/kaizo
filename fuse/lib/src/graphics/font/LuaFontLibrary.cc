#include "private/LuaFontLoader.h"
#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/font/FontWriter.h>
#include <fuse/graphics/font/LuaFontLibrary.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/lua/Utilities.h>
#include <sol.hpp>

namespace fuse::graphics {

auto Font_load(const std::string& path, sol::this_state state) -> Font
{
    sol::state_view lua{state};
    sol::unsafe_function_result result = lua.script_file(path);
    if (result.get_type() == sol::type::table)
    {
        LuaFontLoader loader;
        auto const& fontDesc = result.get<const sol::table&>();
        if (auto maybeFont = loader.loadFont(fontDesc))
        {
            return *maybeFont;
        }
        else
        {
            throw std::runtime_error{"could not load font from '" + path + "'"};
        }
    }
    throw std::runtime_error{"file '" + path + "' return the wrong table (table expected)"};
}

auto Font_makeWriter(const Font& font) -> FontWriter
{
    FontWriter writer;
    writer.setFont(&font);
    return writer;
}

void FontWriter_write(const FontWriter& writer, const std::string& text, Image& image, unsigned x,
                      unsigned y)
{
    writer.write(text, image, x, y, FontWriter::Anchor::TopLeft);
}

auto Font_glyph(const Font& font, const std::string& characters) -> const Glyph*
{
    if (auto maybeGlyph = font.find(characters))
    {
        return &font.glyph(*maybeGlyph);
    }
    else
    {
        return nullptr;
    }
}

auto openFontLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua(state);

    sol::table module = lua.create_table();
    module.new_usertype<Glyph>("Glyph", "advance_width", sol::property(&Glyph::advanceWidth));
    module.new_usertype<Font>("Font", "load", sol::factories(&Font_load), "makewriter",
                              Font_makeWriter, "getglyph", &Font_glyph);
    module.new_usertype<FontWriter>("FontWriter", "write", &FontWriter_write);

    return module;
}

} // namespace fuse::graphics