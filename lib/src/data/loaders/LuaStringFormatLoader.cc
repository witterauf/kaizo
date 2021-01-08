#include "LuaStringFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/formats/StringFormat.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

auto LuaStringFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<StringFormat>>
{
    m_lua = &state;
    std::unique_ptr<StringFormat> stringFormat;
    if (auto maybeEncoding = format.get<sol::optional<std::shared_ptr<text::TextEncoding>>>("encoding"))
    {
        stringFormat = std::make_unique<StringFormat>(*maybeEncoding);
        readDataFormat(format, state, *stringFormat);
    }
    else
    {
        return {};
    }
    if (loadFixedLength(format, *stringFormat))
    {
        return std::move(stringFormat);
    }
    return {};
}

bool LuaStringFormatLoader::loadFixedLength(const sol::table& table, StringFormat& format)
{
    if (hasField(table, "fixed_length"))
    {
        if (auto maybeLength = requireField<size_t>(table, "fixed_length"))
        {
            format.setFixedLength(*maybeLength);
            return true;
        }
        return false;
    }
    return true;
}

//##[ diagnostics ]################################################################################

void LuaStringFormatLoader::reportUnknownEncoding(const std::string& name)
{
    if (hasReporter())
    {
        reporter()
            .report("'%0' is not a known encoding")
            .level(DiagnosticLevel::Error)
            .tag(DiagnosticTags::UnknownEncoding)
            .substitute(name);
    }
}

} // namespace fuse::binary