#pragma once

#include <map>
#include <optional>
#include <sol.hpp>
#include <string>
#include <vector>

namespace diagnostics {
class DiagnosticsReporter;
}

namespace fuse::lua {

class LuaLoader
{
public:
    struct DiagnosticsTags
    {
        static constexpr char FieldMissing[] = "LuaLoader.FieldMissing";
        static constexpr char FieldWrongType[] = "LuaLoader.FieldWrongType";
    };

    void setReporter(diagnostics::DiagnosticsReporter* reporter);
    bool hasReporter() const;
    auto reporter() -> diagnostics::DiagnosticsReporter&;

    template <class T, class U>
    auto requireField(const sol::table&, const U& name) -> std::optional<T>;
    template <class T, class U>
    auto readField(const sol::table&, const U& name) -> std::optional<T>;
    template <class Key> bool hasField(const sol::table& table, const Key& key) const;

    template <class T> auto readArray(const sol::table& table) -> std::optional<std::vector<T>>;
    template <class Key, class Value>
    auto readMap(const sol::table& table) -> std::optional<std::map<Key, Value>>;

private:
    void reportFieldMissing(const std::string& field);
    void reportFieldWrongType(const std::string& field);
    void reportFieldMissing(size_t index);
    void reportFieldWrongType(size_t index);

    diagnostics::DiagnosticsReporter* m_reporter{nullptr};
};

//##[ implementation ]#############################################################################

template <class T, class U>
auto LuaLoader::requireField(const sol::table& table, const U& name) -> std::optional<T>
{
    auto field = table.get<sol::object>(name);
    if (field.valid())
    {
        if (field.is<T>())
        {
            return field.as<T>();
        }
        else
        {
            reportFieldWrongType(name);
        }
    }
    else
    {
        reportFieldMissing(name);
    }
    return {};
}

template <class T, class U>
auto LuaLoader::readField(const sol::table& table, const U& name) -> std::optional<T>
{
    auto field = table.get<sol::object>(name);
    if (field.valid())
    {
        if (field.is<T>())
        {
            return field.as<T>();
        }
        else
        {
            reportFieldWrongType(name);
        }
    }
    return {};
}

template <class Key> bool LuaLoader::hasField(const sol::table& table, const Key& key) const
{
    return table.get<sol::object>(key).valid();
}

template <class T>
auto LuaLoader::readArray(const sol::table& table) -> std::optional<std::vector<T>>
{
    auto const size = table.size();
    bool success{true};
    std::vector<T> array;
    for (auto i = 0U; i < size; ++i)
    {
        if (auto maybeString = requireField<T>(table, i + 1))
        {
            array.push_back(*maybeString);
        }
        else
        {
            success = false;
        }
    }
    if (success)
    {
        return array;
    }
    else
    {
        return {};
    }
}

template <class Key, class Value>
auto LuaLoader::readMap(const sol::table& table) -> std::optional<std::map<Key, Value>>
{
    std::map<Key, Value> map;
    bool success{true};
    for (auto const& keyValue : table)
    {
        if (auto maybeKey = keyValue.first.as<sol::optional<Key>>())
        {
            if (auto maybeValue = requireField<Value>(table, *maybeKey))
            {
                map[*maybeKey] = *maybeValue;
            }
            else
            {
                success = false;
            }
        }
    }
    if (success)
    {
        return map;
    }
    else
    {
        return {};
    }
}

} // namespace fuse::lua