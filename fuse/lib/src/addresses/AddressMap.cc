#include <fuse/addresses/AddressMap.h>

namespace fuse {

bool AddressMap::coversSourceAddress(const Address address) const
{
    return toTargetAddress(address).has_value();
}

bool AddressMap::coversTargetAddress(const Address address) const
{
    return !toSourceAddresses(address).empty();
}

} // namespace fuse