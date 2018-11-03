#pragma once

#include <fuse/lua/LuaLoader.h>
#include <map>
#include <memory>
#include <optional>
#include <sol_forward.hpp>

namespace fuse::binary {

class DataFormat;
class LuaDataFormatLoader;

class LuaFormatLoader : public ::fuse::lua::LuaLoader
{
public:
    void setOtherFormatLoader(LuaDataFormatLoader* loader);
    virtual auto load(const sol::table& format) -> std::optional<std::unique_ptr<DataFormat>> = 0;

protected:
    auto loadOtherFormat(const sol::table& format) -> std::optional<std::unique_ptr<DataFormat>>;

private:
    LuaDataFormatLoader* m_loader;
};

class LuaDataFormatLoader : public LuaFormatLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char UnknownFormat[] = "LuaDataFormatLoader.UnknownFormat";
    };

    LuaDataFormatLoader();

    void registerFormat(const std::string& name, std::unique_ptr<LuaFormatLoader>&& loader);
    auto load(const sol::table& format) -> std::optional<std::unique_ptr<DataFormat>> override;

private:
    std::map<std::string, std::unique_ptr<LuaFormatLoader>> m_loaders;

    void reportUnknownFormat(const std::string& name);
};

} // namespace fuse::binary