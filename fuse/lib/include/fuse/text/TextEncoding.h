#pragma once

#include <fuse/BinaryView.h>
#include <memory>

namespace fuse::text {

class TextEncoding
{
public:
    virtual ~TextEncoding() = default;
    /// Return whether this instance is able to encode strings.
    virtual bool canEncode() const = 0;
    virtual auto encode(const std::string& text) -> Binary = 0;
    /// Return whether this instance is able to decode bytes.
    virtual bool canDecode() const = 0;
    virtual auto decode(const BinaryView& binary, size_t offset)
        -> std::pair<size_t, std::string> = 0;
    virtual auto copy() const -> std::unique_ptr<TextEncoding> = 0;
};

} // namespace fuse::text