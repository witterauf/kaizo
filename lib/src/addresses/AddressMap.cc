#include <kaizo/addresses/AddressMap.h>

namespace kaizo::data {

bool AddressMap::coversSourceAddress(const Address address) const
{
    return toTargetAddress(address).has_value();
}

bool AddressMap::coversTargetAddress(const Address address) const
{
    return !toSourceAddresses(address).empty();
}

} // namespace kaizo::data