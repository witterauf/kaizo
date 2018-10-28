#include "private/LuaFontLoader.h"
#include <diagnostics/Contracts.h>
#include <fuse/graphics/GraphicsLuaLibrary.h>
#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/font/FontLoader.h>
#include <fuse/graphics/font/Glyph.h>
#include <fuse/graphics/image/Image.h>
#include <sol.hpp>

namespace fs = std::filesystem;

namespace fuse::graphics {

namespace {

} // namespace

auto FontLoader::loadFromFile(std::filesystem::path& filename) -> std::optional<Font>
{
    sol::state lua;
    lua.require("graphics", sol::c_call<decltype(&openGraphicsLibrary), &openGraphicsLibrary>);

    auto result = lua.script_file(filename.string());
    if (result.valid())
    {
        if (result.get_type() == sol::type::table)
        {
            sol::table table = result;
            LuaFontLoader loader;
            loader.setBasePath(filename.parent_path());
            return loader.loadFont(table);
        }
    }
    return {};
}

} // namespace fuse::graphics