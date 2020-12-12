#pragma once

#include <kaizo/graphics/ImageFileFormat.h>

namespace kaizo {

class PngFileFormat : public ImageFileFormat
{
public:
    auto load(const std::filesystem::path& path)
        -> std::optional<std::pair<Tile, std::optional<Palette>>> override;
    bool save(const std::filesystem::path& path, const Tile& image,
              const Palette* palette) override;
};

} // namespace kaizo