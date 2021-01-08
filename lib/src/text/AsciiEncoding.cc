#include <kaizo/text/AsciiEncoding.h>

namespace kaizo::data::text {

bool AsciiEncoding::canEncode() const
{
    return true;
}

auto AsciiEncoding::encode(const std::string& text) -> Binary
{
    Binary binary{text.length() + 1};
    for (auto i = 0U; i < text.length(); ++i)
    {
        binary[i] = text[i];
    }
    binary[text.length()] = 0;
    return binary;
}

bool AsciiEncoding::canDecode() const
{
    return true;
}

auto AsciiEncoding::decode(const BinaryView& binary, size_t offset)
    -> std::pair<size_t, std::string>
{
    std::string text;
    while (offset < binary.size())
    {
        if (binary[offset] == 0)
        {
            break;
        }
        text += binary[offset++];
    }
    return {offset, text};
}

auto AsciiEncoding::copy() const -> std::unique_ptr<TextEncoding>
{
    return std::make_unique<AsciiEncoding>();
}

} // namespace kaizo::data::text