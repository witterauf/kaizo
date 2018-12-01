#include "LuaStringFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/formats/StringFormat.h>
#include <fuse/text/LuaTableReader.h>
#include <fuse/text/TableEncoding.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

auto LuaStringFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<StringFormat>>
{
    m_lua = &state;
    std::unique_ptr<StringFormat> stringFormat;
    if (auto maybeEncoding = format.get<sol::optional<text::TextEncoding*>>("encoding"))
    {
        stringFormat = std::make_unique<StringFormat>((*maybeEncoding)->copy());
        readDataFormat(format, state, *stringFormat);
    }
    else if (auto maybeEncodingName = requireField<std::string>(format, "encoding"))
    {
        if (*maybeEncodingName == "table")
        {
            if (auto maybeTableFormat = loadTableFormat(format))
            {
                stringFormat = std::move(*maybeTableFormat);
            }
        }
        else
        {
            reportUnknownEncoding(*maybeEncodingName);
            return {};
        }
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