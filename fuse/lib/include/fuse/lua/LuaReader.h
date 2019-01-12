#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sol.hpp>
#include <stack>
#include <string>

namespace fuse {

class DeserializationConsumer
{
public:
    virtual ~DeserializationConsumer() = default;

    virtual void enterArray(size_t)
    {
    }
    virtual void enterRecord()
    {
    }
    virtual bool enterField(const std::string&)
    {
        return false;
    }
    virtual bool enterElement(size_t)
    {
        return false;
    }
    virtual void consumeInteger(uint64_t)
    {
    }
    virtual void consumeInteger(int64_t)
    {
    }
    virtual void consumeString(const char*)
    {
    }
    virtual void consumeReal(double)
    {
    }
    virtual void leaveElement()
    {
    }
    virtual void leaveRecord()
    {
    }
    virtual void leaveArray()
    {
    }
};

class LuaReader
{
public:
    void open(const std::filesystem::path& fileName);
    void open(const sol::object& data);

    void deserialize(DeserializationConsumer* consumer);
    void finish();

private:
    void deserializeTable(const sol::table& data);
    void deserializeArray(const sol::table& array);
    void deserializeRecord(const sol::table& record);
    void deserializeObject(const sol::object& object);
    void deserializeNumber(const sol::object& number);

    auto consumer() -> DeserializationConsumer&;

    std::stack<sol::object> m_data;
    std::stack<DeserializationConsumer*> m_consumers;
};

} // namespace fuse