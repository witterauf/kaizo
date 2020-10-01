#include "PngFormat.h"
#include <algorithm>
#include <cctype>
#include <functional>
#include <fuse/graphics/image/ImageFormat.h>
#include <map>

namespace {

auto toLower(std::string string) -> std::string
{
    std::transform(string.begin(), string.end(), string.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return std::move(string);
}

} // namespace

namespace fuse::graphics {

const std::map<std::string, std::function<std::unique_ptr<ImageFormat>()>> Formats = {
    {"png", []() { return std::make_unique<PngFormat>(); }}};

auto ImageFormat::makeFormat(const std::string& extension) -> std::unique_ptr<ImageFormat>
{
    auto const format = Formats.find(toLower(extension));
    if (format != Formats.cend())
    {
        return format->second();
    }
    else
    {
        return {};
    }
}

} // namespace fuse::graphics