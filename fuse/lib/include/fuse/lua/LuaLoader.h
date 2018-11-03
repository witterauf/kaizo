#pragma once

#include <optional>
#include <sol_forward.hpp>
#include <string>

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

private:
    void reportFieldMissing(const std::string& field);
    void reportFieldWrongType(const std::string& field);
    void reportFieldMissing(size_t index);
    void reportFieldWrongType(size_t index);

    diagnostics::DiagnosticsReporter* m_reporter;
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

} // namespace fuse::lua