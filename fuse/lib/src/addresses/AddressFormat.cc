#include <fuse/addresses/Address.h>
#include <fuse/addresses/AddressFormat.h>
#include <typeinfo>

namespace fuse {

auto AddressFormat::makeAddress(address_t address) const -> Address
{
    return Address{address, this};
}

bool AddressFormat::isCompatible(const AddressFormat& format) const
{
    return typeid(format) == typeid(*this);
}

} // namespace fuse