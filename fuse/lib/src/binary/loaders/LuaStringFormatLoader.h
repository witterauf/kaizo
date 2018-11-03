#pragma once

#include <fuse/binary/LuaDataFormatLoader.h>
#include <fuse/binary/StringFormat.h>
#include <fuse/text/Table.h>

namespace fuse::binary {

class LuaStringFormatLoader : public LuaFormatLoader
{
public:
    struct DiagnosticTags
    {
        static constexpr char UnknownEncoding[] = "LuaStringFormatLoader.UnknownEncoding";
    };

    auto load(const sol::table& format, sol::this_state state)
        -> std::optional<std::unique_ptr<DataFormat>> override;
    auto loadTableFormat(const sol::table& format) -> std::optional<std::unique_ptr<DataFormat>>;
    auto loadTable(const std::filesystem::path& filename) -> std::optional<text::Table>;

private:
    void reportUnknownEncoding(const std::string& name);

    sol::this_state* m_lua{nullptr};
};

} // namespace fuse::binary