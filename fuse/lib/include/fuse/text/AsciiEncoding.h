#pragma once

#include "TextEncoding.h"

namespace kaizo::data::text {

class AsciiEncoding : public TextEncoding
{
public:
    bool canEncode() const override;
    auto encode(const std::string& text) -> Binary override;
    bool canDecode() const override;
    auto decode(const BinaryView& binary, size_t offset) -> std::pair<size_t, std::string> override;
    auto copy() const -> std::unique_ptr<TextEncoding> override;
};

} // namespace kaizo::data::text