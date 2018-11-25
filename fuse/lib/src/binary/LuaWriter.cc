#include <diagnostics/Contracts.h>
#include <fuse/binary/LuaWriter.h>
#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/data/BinaryData.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/NullData.h>
#include <fuse/binary/data/RecordData.h>
#include <fuse/binary/data/StringData.h>

namespace fuse::binary {

static auto tabs(size_t level) -> std::string
{
    return std::string(level * 2, ' ');
}

auto LuaWriter::format(const std::string& lua) -> std::string
{
    size_t index{0};
    size_t tabLevel{0};
    std::string formatted;
    while (index < lua.length())
    {
        if (lua[index] == '\n')
        {
            formatted += "\n" + tabs(tabLevel);
        }
        else if (lua[index] == '{')
        {
            formatted += lua[index];
            tabLevel += 1;
        }
        else if (lua[index] == '}')
        {
            formatted += lua[index];
            tabLevel -= 1;
        }
        else
        {
            formatted += lua[index];
        }
        index++;
    }
    return formatted;
}

auto LuaWriter::write(const Data& data) -> std::string
{
    switch (data.type())
    {
    case DataType::Null: return write(static_cast<const NullData&>(data));
    case DataType::String: return write(static_cast<const StringData&>(data));
    case DataType::Record: return write(static_cast<const RecordData&>(data));
    case DataType::Binary: return write(static_cast<const BinaryData&>(data));
    case DataType::Array: return write(static_cast<const ArrayData&>(data));
    case DataType::Integer: return write(static_cast<const IntegerData&>(data));
    default: InvalidCase(data.type());
    }
}

auto LuaWriter::write(const NullData&) -> std::string
{
    return "nil";
}

auto LuaWriter::write(const StringData& data) -> std::string
{
    return "[[" + data.value() + "]]";
}

auto LuaWriter::write(const RecordData& data) -> std::string
{
    std::string lua = "{\n";

    auto const fields = data.elementNames();
    for (auto const& fieldName : fields)
    {
        auto const& field = data.element(fieldName);
        lua += fieldName + " = ";
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
    std::string lua = "Binary{\n";
    for (auto i = 0U; i < data.size(); ++i)
    {
        if (i != 0 && (i % 8) == 0)
        {
            lua += "\n";
        }
        lua += "0x" + toHex(data.data()[i]) + ", ";
    }
    lua += "\n}";
    return lua;
}

auto LuaWriter::write(const ArrayData& data) -> std::string
{
    std::string lua = "{\n";
    for (auto i = 0U; i < data.elementCount(); ++i)
    {
        lua += "[" + std::to_string(i + 1) + "] = " + write(data.element(i)) + ",\n";
    }
    lua += "}\n";
    return lua;
}

auto LuaWriter::write(const IntegerData& data) -> std::string
{
    std::string lua;
    if (data.isNegative())
    {
        lua += std::to_string(data.asSigned());
    }
    else
    {
        lua += std::to_string(data.asUnsigned());
    }
    return lua;
}

} // namespace fuse::binary