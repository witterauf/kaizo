#include <Catch2.hpp>
#include <fuse/graphics/GraphicsLuaLibrary.h>
#include <fuse/graphics/font/FontWriter.h>
#include <graphics/font/private/LuaFontLoader.h>
#include <sol.hpp>

using namespace fuse::graphics;

SCENARIO("Loading a bitmap font from a .lua file", "[Lua]")
{
    LuaFontLoader loader;
    loader.setBasePath("c:\\build");
    sol::state lua;
    lua["graphics"] = openGraphicsLibrary(lua.lua_state());
    auto result = lua.script_file("c:\\build\\test_font.lua");
    if (result.valid() && result.get_type() == sol::type::table)
    {
        if (auto maybeFont = loader.loadFont(result.get<sol::table>()))
        {
            FontWriter writer;
            writer.setFont(&*maybeFont);
            auto const boundingBox = writer.boundingBox("abcdefghijklmnopqrstuvwxyz");
            Image image{boundingBox.width(), boundingBox.height(), Image::PixelFormat::RGBA, 32};
            writer.write("abcdefghijklmnopqrstuvwxyz", image, 0, 0);
            image.save("c:\\build\\font_writer_test.png");
            return;
        }
    }
}