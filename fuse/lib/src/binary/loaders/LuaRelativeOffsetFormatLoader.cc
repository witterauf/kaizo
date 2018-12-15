#include "LuaRelativeOffsetFormatLoader.h"
#include "../../LuaIntegerLayoutLoader.h"
#include <fuse/addresses/AddressFormat.h>
#include <sol.hpp>

namespace fuse::binary {

auto LuaRelativeOffsetFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<RelativeOffsetFormat>>
{
    auto pointerFormat = std::make_unique<RelativeOffsetFormat>();
    if (loadOffsetFormat(format, *pointerFormat) && loadAddressFormat(format, *pointerFormat) &&
        loadBaseAddress(format, *pointerFormat) && loadPointeeFormat(format, *pointerFormat) &&
        loadNullPointer(format, *pointerFormat) && loadUseAddressMap(format, *pointerFormat))
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
    if (auto maybeOffsetFormat = requireField<sol::table>(table, "offset_format"))
    {
        auto layout = loadIntegerLayout(*maybeOffsetFormat);
        auto storageFormat = std::make_unique<RelativeStorageFormat>();
        storageFormat->setOffsetFormat(layout);
        format.setOffsetFormat(std::move(storageFormat));       
        return true;
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadAddressFormat(const sol::table& table,
                                                      RelativeOffsetFormat& format)
{
    if (auto maybeAddressFormat = requireField<AddressFormat*>(table, "address_format"))
    {
        format.setAddressFormat(*maybeAddressFormat);
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
            if (auto maybeAddress = format.addressFormat().fromInteger(field.as<int64_t>()))
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
    if (hasField(table, "pointee_format"))
    {
        if (auto maybeDataFormat = readField<DataFormat*>(table, "pointee_format"))
        {
            format.setPointedFormat((*maybeDataFormat)->copy());
            return true;
        }
        else if (auto maybeString = readField<std::string>(table, "pointee_format"))
        {
            if (*maybeString == "reference")
            {
                return true;
            }
        }
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadNullPointer(const sol::table& table,
                                                    RelativeOffsetFormat& format)
{
    if (hasField(table, "null_pointer"))
    {
        if (auto maybeNullPointer = requireField<sol::table>(table, "null_pointer"))
        {
            if (auto maybeAddress = requireField<sol::object>(*maybeNullPointer, "address"))
            {
                if (maybeAddress->is<Address>())
                {
                    format.setNullPointer(maybeAddress->as<Address>());
                }
                else if (maybeAddress->is<AddressFormat::address_t>())
                {
                    if (auto const address = format.addressFormat().fromInteger(
                            maybeAddress->as<AddressFormat::address_t>()))
                    {
                        format.setNullPointer(*address);
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }

            if (auto maybeOffset =
                    requireField<AddressFormat::offset_t>(*maybeNullPointer, "offset"))
            {
                format.setNullPointerOffset(*maybeOffset);
            }
            else
            {
                return false;
            }

            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool LuaRelativeOffsetFormatLoader::loadUseAddressMap(const sol::table& table,
                                                      RelativeOffsetFormat& format)
{
    if (hasField(table, "use_address_map"))
    {
        auto field = table.get<sol::object>("use_address_map");
        if (field.is<bool>())
        {
            format.useAddressMap(field.as<bool>());
        }
        else
        {
            return false;
        }
    }
    return true;
}

} // namespace fuse::binary