#include "LuaFontLoader.h"
#include <diagnostics/Contracts.h>
#include <fuse/graphics/tiles/TileColorRemapper.h>
#include <sol.hpp>

namespace fuse::graphics {

void LuaFontLoader::setBasePath(const std::filesystem::path& path)
{
    m_basePath = path;
}

auto LuaFontLoader::loadImage(const std::string& name) -> std::optional<Image>
{
    if (auto maybeBitmap = Image::load(m_basePath / name))
    {
        return maybeBitmap;
    }
    else
    {
        return {};
    }
}

auto LuaFontLoader::loadBitmapPaths(const sol::table& table)
    -> std::optional<std::vector<std::string>>
{
    std::vector<std::string> paths;
    for (auto i = 1U; i <= table.size(); ++i)
    {
        if (auto maybePath = table.get<sol::optional<std::string>>(i))
        {
            paths.push_back(*maybePath);
        }
        else
        {
            return {};
        }
    }
    return paths;
}

auto LuaFontLoader::loadPixelConversion(const sol::table& table)
    -> std::optional<std::unique_ptr<TileTransformation>>
{
    Expects(table.valid());

    if (auto maybeColorMap = table.get<sol::optional<sol::table>>("color_map"))
    {
        auto transformation = std::make_unique<TileColorRemapper>();

        for (auto mapping : *maybeColorMap)
        {
            if (mapping.first.is<uint32_t>() && mapping.second.is<uint32_t>())
            {
                const uint32_t keyColor = mapping.first.as<uint32_t>();
                const uint32_t valueColor = mapping.second.as<uint32_t>();
                transformation->addMapping(keyColor, valueColor);
            }
        }
        return std::move(transformation);
    }
    return {};
}

auto LuaFontLoader::loadMetrics(const sol::table& table) -> std::optional<FontBuilder::FontMetrics>
{
    auto maybeLineHeight = table.get<sol::optional<size_t>>("line_height");
    auto maybeBaseLine = table.get<sol::optional<size_t>>("base_line");
    if (maybeLineHeight && maybeBaseLine)
    {
        FontBuilder::FontMetrics metrics;
        metrics.lineHeight = *maybeLineHeight;
        metrics.baseLine = *maybeBaseLine;
        return metrics;
    }
    return {};
}

auto LuaFontLoader::loadGlyphs(const sol::table& table)
    -> std::optional<std::vector<FontBuilder::GlyphDescriptor>>
{
    std::vector<FontBuilder::GlyphDescriptor> glyphDescriptors;
    for (auto i = 1U; i <= table.size(); ++i)
    {
        if (auto maybeGlyphTable = table.get<sol::optional<sol::table>>(i))
        {
            if (auto maybeGlyph = loadGlyph(*maybeGlyphTable))
            {
                glyphDescriptors.push_back(*maybeGlyph);
            }
            else
            {
                return {};
            }
        }
    }
    return glyphDescriptors;
}

auto LuaFontLoader::loadBoundingBox(const sol::table& table) -> std::optional<TileRegion>
{
    if (table.size() == 4)
    {
        TileRegion region;
        region.setLeft(table.get<size_t>(1));
        region.setTop(table.get<size_t>(2));
        region.setRight(table.get<size_t>(3));
        region.setBottom(table.get<size_t>(4));
        return region;
    }
    return {};
}

auto LuaFontLoader::loadGlyph(const sol::table& table)
    -> std::optional<FontBuilder::GlyphDescriptor>
{
    auto maybeCharacters = table.get<sol::optional<std::string>>("characters");
    auto maybeBaseLine = table.get<sol::optional<size_t>>("base_line");

    if (!maybeCharacters || !maybeBaseLine)
    {
        return {};
    }

    if (auto maybeBoundingBoxTable = table.get<sol::optional<sol::table>>("bounding_box"))
    {
        if (auto maybeBoundingBox = loadBoundingBox(*maybeBoundingBoxTable))
        {
            FontBuilder::GlyphDescriptor glyph;
            glyph.shrinkToFit = table.get_or("shrink", true);
            glyph.bitmapIndex = table.get_or("bitmap", 1ULL);
            glyph.baseLine = *maybeBaseLine;
            glyph.boundingBox = *maybeBoundingBox;
            glyph.characters = *maybeCharacters;
            return glyph;
        }
    }
    return {};
}

auto LuaFontLoader::loadGlyphPixelFormat(const sol::table& table)
    -> std::optional<FontBuilder::GlyphPixelFormat>
{
    auto maybeBackgroundColor = requireField<Glyph::pixel_t>(table, "background_color");
    auto maybeBitsPerPixel = requireField<unsigned int>(table, "bits_per_pixel");
    if (maybeBackgroundColor && maybeBitsPerPixel)
    {
        FontBuilder::GlyphPixelFormat format;
        format.backgroundColor = *maybeBackgroundColor;
        format.bitsPerPixel = *maybeBitsPerPixel;
        return format;
    }
    return {};
}

auto LuaFontLoader::loadFont(sol::table table) -> std::optional<Font>
{
    FontBuilder builder;

    bool imageSuccess{false};
    if (auto maybeBitmapsTable = requireField<sol::table>(table, "bitmaps"))
    {
        if (auto maybeBitmapPaths = loadBitmapPaths(*maybeBitmapsTable))
        {
            imageSuccess = true;
            for (auto const& path : *maybeBitmapPaths)
            {
                if (auto maybeBitmap = loadImage(path))
                {
                    builder.appendBitmap(*maybeBitmap);
                }
                else
                {
                    imageSuccess = false;
                    break;
                }
            }
        }
    }

    bool pixelConversionSuccess{true};
    if (hasField(table, "pixel_conversion"))
    {
        pixelConversionSuccess = false;
        if (auto maybePixelConversionTable = readField<sol::table>(table, "pixel_conversion"))
        {
            if (auto maybePixelConversion = loadPixelConversion(*maybePixelConversionTable))
            {
                builder.setPixelTransformation(std::move(*maybePixelConversion));
                pixelConversionSuccess = true;
            }
        }
    }

    bool pixelFormatSuccess{false};
    if (auto maybePixelFormatTable = requireField<sol::table>(table, "glyph_pixel_format"))
    {
        if (auto maybePixelFormat = loadGlyphPixelFormat(*maybePixelFormatTable))
        {
            builder.setGlyphPixelFormat(*maybePixelFormat);
            pixelFormatSuccess = true;
        }
    }

    bool metricsSuccess{false};
    if (auto maybeMetricsTable = requireField<sol::table>(table, "metrics"))
    {
        if (auto maybeMetrics = loadMetrics(*maybeMetricsTable))
        {
            builder.setFontMetrics(*maybeMetrics);
            metricsSuccess = true;
        }
    }

    bool glyphsSuccess{false};
    if (auto maybeGlyphsTable = requireField<sol::table>(table, "glyphs"))
    {
        if (auto maybeGlyphs = loadGlyphs(*maybeGlyphsTable))
        {
            for (auto const& glyph : *maybeGlyphs)
            {
                builder.addGlyph(glyph);
            }
            glyphsSuccess = true;
        }
    }

    if (pixelConversionSuccess && metricsSuccess && glyphsSuccess && imageSuccess &&
        pixelFormatSuccess)
    {
        return builder.build();
    }
    else
    {
        return {};
    }
}

} // namespace fuse::graphics