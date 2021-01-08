#include "LuaArrayFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

auto LuaArrayFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<ArrayFormat>>
{
    auto arrayFormat = std::make_unique<ArrayFormat>();

    bool elementFormatSuccess{false};
    if (auto maybeElementFormat = requireField<DataFormat*>(format, "element_format"))
    {
        arrayFormat->setElementFormat((*maybeElementFormat)->copy());
        elementFormatSuccess = true;
    }

    bool sizeSuccess{true};
    if (hasField(format, "size"))
    {
        if (auto maybeSize = readField<sol::object>(format, "size"))
        {
            if (auto maybeProvider = loadSize(*maybeSize))
            {
                arrayFormat->setSizeProvider(std::move(*maybeProvider));
            }
        }
        else
        {
            sizeSuccess = false;
        }
    }

    bool dataFormatSuccess = readDataFormat(format, state, *arrayFormat);

    if (elementFormatSuccess && sizeSuccess && dataFormatSuccess)
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