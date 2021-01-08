#include "LuaRecordFormatLoader.h"
#include <sol.hpp>

namespace fuse::binary {

auto LuaRecordFormatLoader::load(const sol::table& table, sol::this_state state)
    -> std::optional<std::unique_ptr<RecordFormat>>
{
    auto recordFormat = std::make_unique<RecordFormat>();
    if (loadElements(table, *recordFormat))
    {
        if (readDataFormat(table, state, *recordFormat))
        {
            return std::move(recordFormat);
        }
    }
    return {};
}

bool LuaRecordFormatLoader::loadElements(const sol::table& table, RecordFormat& format)
{
    if (auto maybeElements = requireField<sol::table>(table, "elements"))
    {
        auto const size = maybeElements->size();
        for (auto i = 0U; i < size; ++i)
        {
            if (auto maybeElementTable = requireField<sol::table>(*maybeElements, i + 1))
            {
                if (loadElement(*maybeElementTable, format))
                {
                    continue;
                }
            }
            return false;
        }
        return true;
    }
    return false;
}

bool LuaRecordFormatLoader::loadElement(const sol::table& table, RecordFormat& format)
{
    if (auto maybeName = requireField<std::string>(table, "name"))
    {
        if (auto maybeElementFormat = requireField<DataFormat*>(table, "element_format"))
        {
            if (hasField(table, "fixed_offset"))
            {
                if (auto maybeFixedOffset = readField<size_t>(table, "fixed_offset"))
                {
                    (*maybeElementFormat)->setFixedOffset(*maybeFixedOffset);
                }
                else
                {
                    return false;
                }
            }

            format.append(*maybeName, (*maybeElementFormat)->copy());
            return true;
        }
    }
    return false;
}

} // namespace fuse::binary