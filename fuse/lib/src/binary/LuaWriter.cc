#include <diagnostics/Contracts.h>
#include <fuse/binary/BinaryData.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/LuaWriter.h>
#include <fuse/binary/RecordData.h>
#include <fuse/binary/StringData.h>

namespace fuse::binary {

auto LuaWriter::write(const Data& data) -> std::string
{
    switch (data.type())
    {
    case DataType::String: return write(static_cast<const StringData&>(data));
    case DataType::Record: return write(static_cast<const RecordData&>(data));
    case DataType::Binary: return write(static_cast<const BinaryData&>(data));
    default: InvalidCase(data.type());
    }
}

auto LuaWriter::write(const StringData& data) -> std::string
{
    return "\"" + data.value() + "\"";
}

auto LuaWriter::write(const RecordData& data) -> std::string
{
    std::string lua = "{\n";

    auto const fields = data.elementNames();
    for (auto const& fieldName : fields)
    {
        auto const& field = data.element(fieldName);
        lua += "  " + fieldName + " = ";
        lua += write(field) + ",\n";
    }
    lua += "}";
    return lua;
}

static auto toHex(uint8_t byte) -> std::string
{
    static const char Digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string hex(2, '0');
    hex[0] = Digits[byte & 0xf];
    hex[1] = Digits[byte >> 4];
    return hex;
}

auto LuaWriter::write(const BinaryData& data) -> std::string
{
    std::string lua = "Binary{\n  ";
    for (auto i = 0U; i < data.size(); ++i)
    {
        if (i != 0 && (i % 8) == 0)
        {
            lua += "\n  ";
        }
        lua += "0x" + toHex(data.data()[i]) + ", ";
    }
    lua += "}";
    return lua;
}

} // namespace fuse::binary