#pragma once

#include <fuse/Binary.h>

namespace fuse::psp {

auto swizzle(const Binary& texture, unsigned width, size_t start = 0, size_t end = 0) -> Binary;
auto unswizzle(const Binary& texture, unsigned width, size_t start = 0, size_t end = 0) -> Binary;

} // namespace fuse::psp