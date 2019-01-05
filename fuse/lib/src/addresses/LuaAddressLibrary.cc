#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/Address.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/AddressMap.h>
#include <fuse/addresses/LuaAddressLibrary.h>
#include <fuse/addresses/RegionAddressMap.h>
#include <fuse/lua/Utilities.h>
#include <fuse/utilities/StringAlgorithms.h>
#include <sol.hpp>

using namespace std::literals;

namespace fuse {

auto makeAbsoluteOffset() -> std::unique_ptr<AbsoluteOffset>
{
    return std::make_unique<AbsoluteOffset>();
}

auto fromInteger(const AddressFormat& format, uint64_t integer, sol::this_state state)
    -> sol::object
{
    if (auto maybeAddress = format.fromInteger(integer))
    {
        return sol::make_object(state, *maybeAddress);
    }
    else
    {
        return sol::nil;
    }
}

auto AddressMap_toTarget(const AddressMap& map, const Address address, sol::this_state state)
    -> sol::object
{
    if (auto maybeAddress = map.toTargetAddress(address))
    {
        return sol::make_object(state, *maybeAddress);
    }
    else
    {
        return sol::nil;
    }
}

auto AddressMap_integerToTarget(const AddressMap& map, uint64_t source, sol::this_state state)
    -> sol::object
{
    if (auto maybeSource = map.sourceFormat().fromInteger(source))
    {
        if (auto maybeAddress = map.toTargetAddress(*maybeSource))
        {
            return sol::make_object(state, *maybeAddress);
        }
        else
        {
            return sol::nil;
        }
    }
    throw FuseException{"could not convert " + utilities::toString(source, 16) +
                        " into an address"};
}

auto AddressMap_integerToSource(const AddressMap& map, uint64_t target, sol::this_state state)
    -> sol::object
{
    if (auto maybeTarget = map.targetFormat().fromInteger(target))
    {
        auto const addresses = map.toSourceAddresses(*maybeTarget);
        sol::table result = sol::state_view{state}.create_table();
        for (auto i = 0U; i < addresses.size(); ++i)
        {
            result[i + 1] = addresses[i];
        }
        return result;
    }
    throw FuseException{"could not convert " + utilities::toString(target, 16) +
                        " into an address"};
}

auto AddressMap_toSource(const AddressMap& map, const Address address, sol::this_state state)
    -> sol::object
{
    auto const addresses = map.toSourceAddresses(address);
    sol::table result = sol::state_view{state}.create_table();
    for (auto i = 0U; i < addresses.size(); ++i)
    {
        result[i + 1] = addresses[i];
    }
    return result;
}

auto AddressMap_fromRegions(const sol::table& mapping) -> std::shared_ptr<AddressMap>
{
    auto const* sourceFormat = requireField<AddressFormat*>(mapping, "source_format");
    auto const* targetFormat = requireField<AddressFormat*>(mapping, "target_format");
    auto addressMap = std::make_unique<RegionAddressMap>(sourceFormat, targetFormat);

    if (hasField(mapping, "regions"))
    {
        auto regions = requireField<sol::table>(mapping, "regions");
        auto const size = regions.size();
        for (auto i = 0U; i < size; ++i)
        {
            auto region = requireField<sol::table>(regions, i + 1);
            auto const source = requireField<uint64_t>(region, "source");
            auto const target = requireField<uint64_t>(region, "target");
            auto const regionSize = requireField<size_t>(region, "size");
            if (auto maybeSourceAddress = sourceFormat->fromInteger(source))
            {
                if (auto maybeTargetAddress = targetFormat->fromInteger(target))
                {
                    addressMap->map(*maybeSourceAddress, *maybeTargetAddress, regionSize);
                }
                else
                {
                    throw FuseException{"invalid target address"};
                }
            }
            else
            {
                throw FuseException{"invalid source address"};
            }
        }
    }

    return std::move(addressMap);
}

auto openAddressLibrary(sol::this_state state) -> sol::table
{
    sol::state_view lua{state};
    sol::table module = lua.create_table();

    module.new_usertype<Address>("Address", sol::meta_method::addition, &Address::applyOffset,
                                 sol::meta_method::subtraction, &Address::subtract,
                                 sol::meta_method::to_string, &Address::toString, "is_valid",
                                 &Address::isValid);

    module.new_usertype<AddressFormat>("AddressFormat", "from_integer", &fromInteger);
    module.new_usertype<AbsoluteOffset>("AbsoluteOffset", sol::base_classes,
                                        sol::bases<AddressFormat>());

    module.new_usertype<AddressMap>(
        "AddressMap", "from_regions", sol::factories(&AddressMap_fromRegions), "to_target_address",
        sol::overload(&AddressMap_toTarget, &AddressMap_integerToTarget), "to_source_addresses",
        sol::overload(&AddressMap_toSource, &AddressMap_integerToSource));

    module["FileOffset"] = fileOffsetFormat();

    return module;
}

} // namespace fuse