#pragma once

namespace fuse {

enum class Signedness
{
    Signed,
    Unsigned
};

enum class Endianness
{
    Little,
    Big
};

struct IntegerLayout
{
    size_t sizeInBytes;
    Signedness signedness;
    Endianness endianness;
};

class LuaWriter;
void serialize(LuaWriter& writer, const IntegerLayout& layout);

} // namespace fuse