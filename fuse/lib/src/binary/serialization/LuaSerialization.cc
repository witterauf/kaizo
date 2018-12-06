#include <diagnostics/Contracts.h>
#include <fstream>
#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/data/BinaryData.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/NullData.h>
#include <fuse/binary/data/RecordData.h>
#include <fuse/binary/data/StringData.h>
#include <fuse/binary/serialization/LuaSerialization.h>
#include <fuse/lua/Utilities.h>

using namespace fuse::binary;

namespace fuse {

LuaSerialization::LuaSerialization(sol::this_state state)
    : m_state{state}
{
}

void LuaSerialization::serialize(const binary::Data& data, const std::filesystem::path& filename)
{
    std::ofstream output{filename};
    if (output.good())
    {
        auto const string = write(data);
        output << string;
    }
    else
    {
        throw std::runtime_error{"could not open file '" + filename.string() + "' for writing"};
    }
}

auto LuaSerialization::deserialize(const std::filesystem::path& filename)
    -> std::unique_ptr<binary::Data>
{
    sol::state_view lua{m_state};
    auto result = lua.script_file(filename.string());

    return nullptr;
}

auto LuaSerialization::read(const sol::object& object) -> std::unique_ptr<Data>
{
    switch (object.get_type())
    {
    case sol::type::table: return readComplex(object);
    case sol::type::string: return readString(object);
    case sol::type::number: return readString(object);
    default: throw std::runtime_error{"unsupported data type"};
    }
}

auto LuaSerialization::readString(const sol::object& object) -> std::unique_ptr<binary::Data>
{
    return std::make_unique<StringData>(object.as<std::string>());
}

auto LuaSerialization::readComplex(const sol::object& object) -> std::unique_ptr<binary::Data>
{
    auto const& table = object.as<sol::table>();
    if (hasField(table, "type"))
    {
        auto const type = readField<DataType>(table, "type");
        if (type == DataType::Array)
        {
            return readArray(table);
        }
        else if (type == DataType::Record)
        {
            return readRecord(table);
        }
        throw FuseException{"type '" + toString(type) +
                            "' not supported in this context ('table')"};
    }
    else if (hasField(table, 1))
    {
        return readArray(table);
    }
    else
    {
        return readRecord(table);
    }
}

auto LuaSerialization::readArray(const sol::table& table) -> std::unique_ptr<binary::Data>
{
    auto const size = table.size();
    auto array = std::make_unique<ArrayData>(size);
    for (auto i = 0U; i < size; ++i)
    {
        auto element = read(table.get<sol::object>(i + 1));
        array->set(i, std::move(element));
    }
    return std::move(array);
}

auto LuaSerialization::readRecord(const sol::table& table) -> std::unique_ptr<binary::Data>
{
    auto record = std::make_unique<RecordData>();
    for (auto const& elementPair : table)
    {
        if (elementPair.first.is<std::string>())
        {
            auto const name = elementPair.first.as<std::string>();
            auto element = read(elementPair.second);
            record->set(name, std::move(element));
        }
    }
    return std::move(record);
}

auto LuaSerialization::readInteger(const sol::object& object) -> std::unique_ptr<binary::Data>
{
    auto const integer = object.as<int64_t>();
    if (integer < 0)
    {
        return std::make_unique<IntegerData>(integer);
    }
    else
    {
        return std::make_unique<IntegerData>(object.as<uint64_t>());
    }
}

static auto tabs(size_t level) -> std::string
{
    return std::string(level * 2, ' ');
}

auto LuaSerialization::format(const std::string& lua) -> std::string
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

auto LuaSerialization::write(const Data& data) -> std::string
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

auto LuaSerialization::write(const NullData&) -> std::string
{
    return "nil";
}

auto LuaSerialization::write(const StringData& data) -> std::string
{
    return "[[" + data.value() + "]]";
}

auto LuaSerialization::write(const RecordData& data) -> std::string
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

auto LuaSerialization::write(const BinaryData& data) -> std::string
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

auto LuaSerialization::write(const ArrayData& data) -> std::string
{
    std::string lua = "{\n";
    for (auto i = 0U; i < data.elementCount(); ++i)
    {
        lua += "[" + std::to_string(i + 1) + "] = " + write(data.element(i)) + ",\n";
    }
    lua += "}\n";
    return lua;
}

auto LuaSerialization::write(const IntegerData& data) -> std::string
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

} // namespace fuse