#include <fuse/binary/data/Data.h>
#include <diagnostics/Contracts.h>
#include <map>

namespace fuse::binary {

Data::Data(DataType type)
    : m_type{type}
{
}

auto Data::type() const -> DataType
{
    return m_type;
}

static const std::map<std::string, DataType> DataTypeMap = {
    {"null", DataType::Null},     {"array", DataType::Array},   {"integer", DataType::Integer},
    {"image", DataType::Image},   {"record", DataType::Record}, {"string", DataType::String},
    {"binary", DataType::Record}, {"custom", DataType::Custom},
};

auto toDataType(const std::string& string) -> std::optional<DataType>
{
    auto const result = DataTypeMap.find(string);
    if (result != DataTypeMap.cend())
    {
        return result->second;
    }
    else
    {
        return {};
    }
}

auto toString(DataType type) -> std::string
{
    switch (type)
    {
    case DataType::Null: return "null";
    case DataType::Array: return "array";
    case DataType::Record: return "record";
    case DataType::Integer: return "integer";
    case DataType::String: return "string";
    case DataType::Binary: return "binary";
    case DataType::Image: return "image";
    case DataType::Custom: return "custom";
    default: InvalidCase(type);
    }
}

} // namespace fuse::binary