#pragma once

#include <filesystem>
#include <fuse/graphics/font/Font.h>
#include <fuse/graphics/font/Glyph.h>
#include <fuse/graphics/tiles/TileImageFormat.h>
#include <map>
#include <optional>
#include <string>

namespace fuse::graphics {

class FontLoader
{
public:
    auto loadFromFile(const std::filesystem::path& filename) -> std::optional<Font>;
};

} // namespace fuse::graphics