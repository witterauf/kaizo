#pragma once

#include <fuse/Binary.h>
#include <memory>

namespace fuse::text {

class TextEncoding
{
public:
    virtual ~TextEncoding() = default;
    virtual bool canEncode() const = 0;
    virtual auto encode(const std::string& tex) -> Binary = 0;
    virtual bool canDecode() const = 0;
    virtual auto decode(const Binary& binary, size_t offset) -> std::pair<size_t, std::string> = 0;
    virtual auto copy() const -> std::unique_ptr<TextEncoding> = 0;
};

} // namespace fuse::text