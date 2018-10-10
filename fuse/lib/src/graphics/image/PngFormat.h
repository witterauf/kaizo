#pragma once

#include <fuse/graphics/image/ImageFormat.h>

namespace fuse::graphics {

class PngFormat : public ImageFormat
{
public:
    auto load(const std::filesystem::path& path) -> std::optional<Image> override;
    bool save(const std::filesystem::path& path, const Image& image) override;
};

} // namespace fuse::graphics