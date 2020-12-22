#pragma once

#include <fuse/BinaryView.h>

namespace kaizo {

auto swizzle(const fuse::BinaryView& texture, unsigned width, size_t start = 0, size_t end = 0)
    -> fuse::Binary;
auto unswizzle(const fuse::BinaryView& texture, unsigned width, size_t start = 0, size_t end = 0)
    -> fuse::Binary;

} // namespace kaizo