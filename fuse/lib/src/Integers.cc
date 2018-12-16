#include <diagnostics/Contracts.h>
#include <fuse/Integers.h>
#include <fuse/lua/LuaWriter.h>

namespace fuse {

static auto toEnumName(Signedness signedness) -> std::string
{
    switch (signedness)
    {
    case Signedness::Signed: return "SIGNEDNESS.SIGNED";
    case Signedness::Unsigned: return "SIGNEDNESS.UNSIGNED";
    default: InvalidCase(signedness);
    }
}

static auto toEnumName(Endianness endianness) -> std::string
{
    switch (endianness)
    {
    case Endianness::Big: return "ENDIANNESS.BIG";
    case Endianness::Little: return "ENDIANNESS.LITTLE";
    default: InvalidCase(endianness);
    }
}

void serialize(LuaWriter& writer, const IntegerLayout& layout)
{
    writer.startTable();
    writer.startField("size");
    writer.writeInteger(layout.sizeInBytes);
    writer.finishField();
    writer.startField("signedness");
    writer.writeEnum(toEnumName(layout.signedness));
    writer.finishField();
    writer.startField("endianness");
    writer.writeEnum(toEnumName(layout.endianness));
    writer.finishField();
    writer.finishTable();
}

} // namespace fuse