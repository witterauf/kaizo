#pragma once

#include <filesystem>
#include <kaizo/graphics/Palette.h>
#include <kaizo/graphics/Tile.h>
#include <memory>
#include <optional>
#include <string>

namespace kaizo {

class ImageFileFormat
{
public:
    static auto makeFormat(const std::string& extension) -> std::unique_ptr<ImageFileFormat>;

    virtual ~ImageFileFormat() = default;
    virtual auto load(const std::filesystem::path& path)
        -> std::optional<std::pair<Tile, std::optional<Palette>>> = 0;
    virtual bool save(const std::filesystem::path& path, const Tile& image, const Palette*) = 0;
};

} // namespace kaizo