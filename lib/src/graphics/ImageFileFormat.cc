#include "kaizo/graphics/ImageFileFormat.h"
#include "PngFileFormat.h"
#include <algorithm>
#include <cctype>
#include <functional>
#include <map>

namespace {

auto toLower(std::string string) -> std::string
{
    std::transform(string.begin(), string.end(), string.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return std::move(string);
}

} // namespace

namespace kaizo {

const std::map<std::string, std::function<std::unique_ptr<ImageFileFormat>()>> Formats = {
    {"png", []() { return std::make_unique<PngFileFormat>(); }}};

auto ImageFileFormat::makeFormat(const std::string& extension) -> std::unique_ptr<ImageFileFormat>
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

} // namespace kaizo