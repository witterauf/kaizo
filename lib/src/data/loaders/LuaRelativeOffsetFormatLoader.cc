#include "LuaRelativeOffsetFormatLoader.h"
#include "../../LuaIntegerLayoutLoader.h"
#include <fuse/addresses/AddressFormat.h>
#include <sol.hpp>

namespace fuse::binary {

auto LuaRelativeOffsetFormatLoader::load(const sol::table& format, sol::this_state state)
    -> std::optional<std::unique_ptr<PointerFormat>>
{
    m_addressFormat = nullptr;
    auto pointerFormat = std::make_unique<PointerFormat>();
    if (loadAddressFormat(format, *pointerFormat) && loadLayout(format, *pointerFormat) &&
        loadPointeeFormat(format, *pointerFormat) && loadUseAddressMap(format, *pointerFormat))
    {
        if (readDataFormat(format, state, *pointerFormat))
        {
            return std::move(pointerFormat);
        }
    }
    return {};
}

bool LuaRelativeOffsetFormatLoader::loadLayout(const sol::table& table, PointerFormat& format)
{
    if (auto maybeLayout = requireField<const AddressStorageFormat*>(table, "layout"))
    {
        format.setLayout(std::move((*maybeLayout)->copy()));
        return true;
    }
    return false;
}

bool LuaRelativeOffsetFormatLoader::loadAddressFormat(const sol::table& table,
                                                      PointerFormat& format)
{
    if (auto maybeAddressFormat = requireField<AddressFormat*>(table, "address_format"))
    {
        m_addressFormat = *maybeAddressFormat;
        format.setAddressFormat(*maybeAddressFormat);
        return true;
    }
    return false;
}

/*
bool LuaRelativeOffsetFormatLoader::loadBaseAddress(const sol::table& table, PointerFormat& format)
{
    if (hasField(table, "base"))
    {
        auto field = table.get<sol::object>("base");
        if (field.is<int64_t>() && m_addressFormat)
        {
            if (auto maybeAddress = m_addressFormat->fromInteger(field.as<int64_t>()))
            {
                format.setBaseAddress(*maybeAddress);
                return true;
            }
        }
        else if (auto maybeBaseAddress = requireField<Address>(table, "base"))
        {
            format.setBaseAddress(*maybeBaseAddress);
            return true;
        }
    }
    return false;
}
*/

bool LuaRelativeOffsetFormatLoader::loadPointeeFormat(const sol::table& table,
                                                      PointerFormat& format)
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

/*
bool LuaRelativeOffsetFormatLoader::loadNullPointer(const sol::table& table, PointerFormat& format)
{
    if (hasField(table, "null_pointer"))
    {
        if (auto maybeNullPointer = requireField<sol::table>(table, "null_pointer"))
        {
            std::optional<Address> address;
            if (auto maybeAddress = requireField<sol::object>(*maybeNullPointer, "address"))
            {
                if (maybeAddress->is<Address>())
                {
                    address = maybeAddress->as<Address>();
                }
                else if (maybeAddress->is<AddressFormat::address_t>() && m_addressFormat)
                {
                    if (auto const fromInteger = m_addressFormat->fromInteger(
                            maybeAddress->as<AddressFormat::address_t>()))
                    {
                        address = *fromInteger;
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
                if (address)
                {
                    format.setNullPointer(*address, *maybeOffset);
                }
                else
                {
                    return false;
                }
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
*/

bool LuaRelativeOffsetFormatLoader::loadUseAddressMap(const sol::table& table,
                                                      PointerFormat& format)
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