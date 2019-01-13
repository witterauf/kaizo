#include "FuseLuaException.h"
#include <sol_forward.hpp>

namespace fuse {

template <class Target> void merge(Target& a, const sol::table& b)
{
    for (auto const& pair : b)
    {
        a[pair.first] = pair.second;
    }
}

template <class Key> bool hasField(const sol::table& table, const Key& key)
{
    auto field = table.get<sol::object>(key);
    return field.valid();
}

template <class Value, class Key> auto readField(const sol::table& table, const Key& key) -> Value
{
    auto field = table.get<sol::object>(key);
    if (field.is<Value>())
    {
        return field.as<Value>();
    }
    else
    {
        if constexpr (std::is_integral<Key>::value)
        {
            throw FuseLuaException{"field [" + std::to_string(key) + "] has the wrong type"};
        }
        else
        {
            throw FuseLuaException{"field '" + std::string{key} + "' has the wrong type"};
        }
    }
}

template <class Value, class Key>
auto requireField(const sol::table& table, const Key& key) -> Value
{
    auto field = table.get<sol::object>(key);
    if (field.valid())
    {
        return readField<Value>(table, key);
    }
    else
    {
        if constexpr (std::is_integral<Key>::value)
        {
            throw FuseLuaException{"field [" + std::to_string(key) + "] is required"};
        }
        else
        {
            throw FuseLuaException{"field '" + std::string{key} + "' is required"};
        }
    }
}

} // namespace fuse