#include "LuaStringFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/StringFormat.h>
#include <fuse/text/LuaTableReader.h>
#include <fuse/text/TableEncoding.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

auto LuaStringFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<StringFormat>>
{
    m_lua = &state;    
    if (auto maybeEncoding = format.get<sol::optional<text::TextEncoding*>>("encoding"))
    {
        auto stringFormat = std::make_unique<StringFormat>((*maybeEncoding)->copy());
        readDataFormat(format, state, *stringFormat);
        return std::move(stringFormat);
    }
    else if (auto maybeEncodingName = requireField<std::string>(format, "encoding"))
    {
        if (*maybeEncodingName == "table")
        {
            return loadTableFormat(format);
        }
        else
        {
            reportUnknownEncoding(*maybeEncodingName);
        }
    }
    return {};
}

auto LuaStringFormatLoader::loadTableFormat(const sol::table& format)
    -> std::optional<std::unique_ptr<StringFormat>>
{
    std::unique_ptr<text::TableEncoding> encoding;
    bool tableSuccess{false};
    if (auto maybeFileName = requireField<std::string>(format, "table"))
    {
        if (auto maybeTable = loadTable(*maybeFileName))
        {
            encoding->addTable(std::move(*maybeTable));
            tableSuccess = true;
        }
    }

    if (tableSuccess)
    {
        auto stringFormat = std::make_unique<StringFormat>(std::move(encoding));
        readDataFormat(format, *m_lua, *stringFormat);
        return std::move(stringFormat);
    }
    else
    {
        return {};
    }
}

auto LuaStringFormatLoader::loadTable(const std::filesystem::path& filename)
    -> std::optional<text::Table>
{
    using namespace ::fuse::text;
    LuaTableReader reader;
    if (m_lua && *m_lua)
    {
        return reader.load(filename, *m_lua);
    }
    else
    {
        return reader.load(filename);
    }
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