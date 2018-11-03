#include "loaders/LuaArrayFormatLoader.h"
#include "loaders/LuaIntegerFormatLoader.h"
#include "loaders/LuaStringFormatLoader.h"
#include <diagnostics/Contracts.h>
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/binary/DataFormat.h>
#include <fuse/binary/LuaDataFormatLoader.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

LuaArrayFormatLoader::LuaArrayFormatLoader(LuaDataFormatLoader* loader)
{
    Expects(loader);
    setOtherFormatLoader(loader);
}

void LuaFormatLoader::setOtherFormatLoader(LuaDataFormatLoader* loader)
{
    m_loader = loader;
}

auto LuaFormatLoader::loadOtherFormat(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<DataFormat>>
{
    Expects(m_loader);
    return m_loader->load(format, state);
}

LuaDataFormatLoader::LuaDataFormatLoader()
{
    m_loaders["integer"] = std::make_unique<LuaIntegerFormatLoader>();
    m_loaders["array"] = std::make_unique<LuaArrayFormatLoader>(this);
    m_loaders["string"] = std::make_unique<LuaStringFormatLoader>();
}

void LuaDataFormatLoader::registerFormat(const std::string& name,
                                         std::unique_ptr<LuaFormatLoader>&& loader)
{
    m_loaders[name] = std::move(loader);
}

auto LuaDataFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<DataFormat>>
{
    if (auto maybeFormat = requireField<std::string>(format, "format"))
    {
        auto const iter = m_loaders.find(*maybeFormat);
        if (m_loaders.cend() != iter)
        {
            return iter->second->load(format, state);
        }
        else
        {
            reportUnknownFormat(*maybeFormat);
            return {};
        }
    }
    return {};
}

void LuaDataFormatLoader::reportUnknownFormat(const std::string& name)
{
    if (hasReporter())
    {
        reporter()
            .report("unknown data format: '%0'")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::UnknownFormat)
            .substitute(name);
    }
}

} // namespace fuse::binary