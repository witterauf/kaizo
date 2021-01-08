#include <kaizo/addresses/Address.h>
#include <kaizo/addresses/AddressFormat.h>
#include <typeinfo>

namespace kaizo::data {

auto AddressFormat::makeAddress(address_t address) const -> Address
{
    return Address{address, this};
}

bool AddressFormat::isCompatible(const AddressFormat& format) const
{
    return typeid(format) == typeid(*this);
}

} // namespace kaizo::data