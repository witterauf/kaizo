#include "LuaRelativeOffsetFormatLoader.h"
#include <sol.hpp>

namespace fuse::binary {

auto LuaRelativeOffsetFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<RelativeOffsetFormat>>
{
    auto pointerFormat = std::make_unique<RelativeOffsetFormat>();
    if (loadOffsetFormat(format, *pointerFormat) && loadAddressFormat(format, *pointerFormat) &&
        loadBaseAddress(format, *pointerFormat) && loadPointeeFormat(format, *pointerFormat) &&
        loadIgnoredOffset(format, *pointerFormat))
    {
        if (readDataFormat(format, state, *pointerFormat))
        {
            return std::move(pointerFormat);
        }
    }
    return {};
}

bool LuaRelativeOffsetFormatLoader::loadOffsetFormat(const sol::table& table,
                                                     RelativeOffsetFormat& format)
{
    if (auto maybeOffsetFormat = requireField<IntegerFormat*>(table, "offset_format"))
    {
        auto copy = (*maybeOffsetFormat)->copy();
        format.setOffsetFormat(
            std::unique_ptr<IntegerFormat>(static_cast<IntegerFormat*>(copy.release())));
        return true;
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadAddressFormat(const sol::table& table,
                                                      RelativeOffsetFormat& format)
{
    if (auto maybeAddressFormat = requireField<AddressFormat*>(table, "address_format"))
    {
        format.setAddressFormat((*maybeAddressFormat)->copy());
        return true;
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadBaseAddress(const sol::table& table,
                                                    RelativeOffsetFormat& format)
{
    if (hasField(table, "base"))
    {
        auto field = table.get<sol::object>("base");
        if (field.is<int64_t>())
        {
            if (auto maybeAddress = format.addressFormat().delinearize(field.as<int64_t>()))
            {
                format.setBaseAddressProvider(
                    std::make_unique<FixedBaseAddressProvider>(*maybeAddress));
                return true;
            }
        }
        else if (auto maybeBaseAddress = requireField<Address>(table, "base"))
        {
            format.setBaseAddressProvider(
                std::make_unique<FixedBaseAddressProvider>(*maybeBaseAddress));
            return true;
        }
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadPointeeFormat(const sol::table& table,
                                                      RelativeOffsetFormat& format)
{
    if (auto maybePointeeFormat = requireField<DataFormat*>(table, "pointee_format"))
    {
        format.setPointedFormat((*maybePointeeFormat)->copy());
        return true;
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadIgnoredOffset(const sol::table& table,
                                                      RelativeOffsetFormat& format)
{
    if (hasField(table, "ignore_if"))
    {
        auto field = table.get<sol::object>("ignore_if");
        if (field.is<int64_t>())
        {
            auto const offset = field.as<int64_t>();
            std::unique_ptr<FixedOffsetValidator> validator;
            if (offset < 0)
            {
                validator =
                    std::make_unique<FixedOffsetValidator>(std::make_unique<IntegerData>(offset));
            }
            else
            {
                validator = std::make_unique<FixedOffsetValidator>(
                    std::make_unique<IntegerData>(field.as<uint64_t>()));
            }
            format.setOffsetValidator(std::move(validator));
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

} // namespace fuse::binary