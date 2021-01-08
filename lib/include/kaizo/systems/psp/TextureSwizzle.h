#pragma once

#include <kaizo/binary/BinaryView.h>

namespace kaizo {

auto swizzle(const BinaryView& texture, unsigned width, size_t start = 0, size_t end = 0) -> Binary;
auto unswizzle(const BinaryView& texture, unsigned width, size_t start = 0, size_t end = 0)
    -> Binary;

} // namespace kaizo