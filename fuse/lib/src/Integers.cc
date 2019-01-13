#include <diagnostics/Contracts.h>
#include <fuse/Integers.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>
#include <fuse/utilities/DomReaderHelpers.h>

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
    writer.startField("size").writeInteger(layout.sizeInBytes).finishField();
    writer.startField("signedness").writeEnum(toEnumName(layout.signedness)).finishField();
    if (layout.endianness != Endianness::Little)
    {
        writer.startField("endianness").writeEnum(toEnumName(layout.endianness)).finishField();
    }
    writer.finishTable();
}

auto IntegerLayout::deserialize(LuaDomReader& reader) -> IntegerLayout
{
    Expects(reader.isRecord());
    IntegerLayout layout;
    if (auto maybeEndianness = readUnsignedInteger(reader, "endianness"))
    {
        layout.endianness = static_cast<Endianness>(*maybeEndianness);
    }
    layout.sizeInBytes = requireUnsignedInteger(reader, "size");
    layout.signedness = static_cast<Signedness>(requireUnsignedInteger(reader, "signedness"));
    return layout;
}

} // namespace fuse