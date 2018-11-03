#include "LuaArrayFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

auto LuaArrayFormatLoader::load(const sol::table& format)
    -> std::optional<std::unique_ptr<DataFormat>>
{
    auto arrayFormat = std::make_unique<ArrayFormat>();

    bool elementFormatSuccess{false};
    if (auto maybeTable = requireField<sol::table>(format, "element_format"))
    {
        if (auto maybeElementFormat = loadOtherFormat(*maybeTable))
        {
            arrayFormat->setElementFormat(std::move(*maybeElementFormat));
            elementFormatSuccess = true;
        }
    }

    bool sizeSuccess{false};
    if (auto maybeSize = requireField<sol::object>(format, "size"))
    {
        if (auto maybeProvider = loadSize(*maybeSize))
        {
            arrayFormat->setSizeProvider(std::move(*maybeProvider));
            sizeSuccess = true;
        }
    }

    if (elementFormatSuccess && sizeSuccess)
    {
        return arrayFormat;
    }
    else
    {
        return {};
    }
}

auto LuaArrayFormatLoader::loadSize(const sol::object& object)
    -> std::optional<std::unique_ptr<ArraySizeProvider>>
{
    switch (object.get_type())
    {
    case sol::type::number: return loadFixedSize(object);
    default: break;
    }
    reportInvalidSize();
    return {};
}

auto LuaArrayFormatLoader::loadFixedSize(const sol::object& object)
    -> std::optional<std::unique_ptr<ArraySizeProvider>>
{
    auto const value = object.as<int64_t>();
    if (value < 0)
    {
        reportInvalidSize();
        return {};
    }
    return std::make_unique<FixedSizeProvider>(object.as<size_t>());
}

//##[ diagnostics ]################################################################################

void LuaArrayFormatLoader::reportInvalidSizeType()
{
    if (hasReporter())
    {
        reporter()
            .report("invalid size type (should be a number, table, or function)")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::InvalidSizeType);
    }
}

void LuaArrayFormatLoader::reportInvalidSize()
{
    if (hasReporter())
    {
        reporter()
            .report("invalid size (must be positive)")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::InvalidSize);
    }
}

} // namespace fuse::binary