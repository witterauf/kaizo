#pragma once

#include "Image.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace fuse::graphics {

class ImageFormat
{
public:
    static auto makeFormat(const std::string& extension) -> std::unique_ptr<ImageFormat>;

    virtual auto load(const std::filesystem::path& path) -> std::optional<Image> = 0;
    virtual bool save(const std::filesystem::path& path, const Image& image) = 0;
};

} // namespace fuse::graphics