#include "LuaStringFormatLoader.h"
#include <diagnostics/DiagnosticsReporter.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/StringFormat.h>
#include <fuse/text/LuaTableReader.h>
#include <fuse/text/TableEncoding.h>
#include <sol.hpp>

using namespace diagnostics;

namespace fuse::binary {

namespace {

class OwnerStringFormat : public DataFormat
{
public:
    OwnerStringFormat(std::unique_ptr<text::TextEncoding>&& encoding)
        : m_encoding{std::move(encoding)}
    {
        m_stringFormat.setEncoding(m_encoding.get());
    }

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override
    {
        return m_stringFormat.decode(reader);
    }

private:
    std::unique_ptr<text::TextEncoding> m_encoding;
    StringFormat m_stringFormat;
};

} // namespace

auto LuaStringFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<DataFormat>>
{
    m_lua = &state;
    if (auto maybeEncoding = format.get<sol::optional<text::TextEncoding*>>("encoding"))
    {
        return std::make_unique<OwnerStringFormat>((*maybeEncoding)->copy());
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
    -> std::optional<std::unique_ptr<DataFormat>>
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
        return std::make_unique<OwnerStringFormat>(std::move(encoding));
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