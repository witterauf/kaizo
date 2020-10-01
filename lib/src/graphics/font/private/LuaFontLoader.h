#pragma once

#include "FontBuilder.h"
#include <filesystem>
#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/image/Image.h>
#include <fuse/lua/LuaLoader.h>
#include <map>
#include <memory>
#include <optional>
#include <sol_forward.hpp>
#include <vector>

namespace fuse::graphics {

class TileTransformation;

class LuaFontLoader : public ::fuse::lua::LuaLoader
{
public:
    void setBasePath(const std::filesystem::path& path);
    auto loadFont(sol::table table) -> std::optional<Font>;

    auto loadImage(const std::string& name) -> std::optional<Image>;
    auto loadPixelConversion(const sol::table& table)
        -> std::optional<std::unique_ptr<TileTransformation>>;
    // auto loadColorMap(const sol::table& table) -> std::optional<FontBuilder::ColorMap>;
    auto loadMetrics(const sol::table& table) -> std::optional<FontBuilder::FontMetrics>;
    auto loadGlyphs(const sol::table& table)
        -> std::optional<std::vector<FontBuilder::GlyphDescriptor>>;
    auto loadGlyph(const sol::table& table) -> std::optional<FontBuilder::GlyphDescriptor>;
    auto loadBoundingBox(const sol::table& table) -> std::optional<TileRegion>;
    auto loadGlyphPixelFormat(const sol::table& table)
        -> std::optional<FontBuilder::GlyphPixelFormat>;
    auto loadBitmapPaths(const sol::table& table)
        -> std::optional<std::vector<std::string>>;

private:
    std::filesystem::path m_basePath;
};

} // namespace fuse::graphics