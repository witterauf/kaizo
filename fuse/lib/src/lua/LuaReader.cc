#include <diagnostics/Contracts.h>
#include <fuse/FuseException.h>
#include <fuse/lua/LuaReader.h>

namespace fuse {

void LuaReader::open(const std::filesystem::path& fileName)
{
    std::ifstream input{fileName, std::ifstream::binary};
    if (input.bad())
    {
        throw FuseException{"could not open file '" + fileName.string() + "' for reading"};
    }
}

void LuaReader::open(const sol::object& data)
{
    Expects(m_data.empty());
    m_data.push(data);
}

void LuaReader::deserialize(DeserializationConsumer* consumer)
{
    m_consumers.push(consumer);
    deserializeObject(m_data.top());
    m_consumers.pop();
}

void LuaReader::deserializeTable(const sol::table& data)
{
    if (data.size() > 0)
    {
        deserializeArray(data);
    }
    else
    {
        deserializeRecord(data);
    }
}

void LuaReader::deserializeArray(const sol::table& array)
{
    auto const length = array.size();
    consumer().enterArray(length);
    for (auto i = 0U; i < length; ++i)
    {
        auto object = array.get<sol::object>(i + 1);
        m_data.push(object);
        if (consumer().enterElement(i))
        {
            deserializeObject(object);
        }
        m_data.pop();
    }
    consumer().leaveArray();
}

void LuaReader::deserializeRecord(const sol::table& record)
{
    consumer().enterRecord();
    for (auto const& pair : record)
    {
        if (pair.first.is<std::string>())
        {
            m_data.push(pair.second);
            if (consumer().enterField(pair.first.as<std::string>()))
            {
                deserializeObject(pair.second);
            }
            m_data.pop();
        }
    }
    consumer().leaveRecord();
}

void LuaReader::deserializeObject(const sol::object& object)
{
    switch (object.get_type())
    {
    case sol::type::number: deserializeNumber(object); break;
    case sol::type::string: consumer().consumeString(object.as<const char*>()); break;
    case sol::type::table: deserializeTable(object.as<sol::table>()); break;
    case sol::type::nil: throw FuseException("nil not supported in deserialization");
    default: throw FuseException{"unsupported type for deserialization"};
    }
}

void LuaReader::deserializeNumber(const sol::object& number)
{
    if (number.is<int64_t>())
    {
        consumer().consumeInteger(number.as<int64_t>());
    }
    else
    {
        consumer().consumeReal(number.as<double>());
    }
}

void LuaReader::finish()
{
    Expects(!m_consumers.empty());
    m_consumers.pop();
}

auto LuaReader::consumer() -> DeserializationConsumer&
{
    return *m_consumers.top();
}

} // namespace fuse