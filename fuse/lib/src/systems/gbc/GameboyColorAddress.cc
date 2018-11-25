#include <fuse/Binary.h>
#include <fuse/addresses/Address.h>
#include <fuse/systems/gbc/GameboyColorAddress.h>

namespace fuse::systems::gbc {

auto GameboyColorAddressFormat::applyOffset(const Address& address, int64_t offset) const -> Address
{
    return makeAddress(address.toInteger() + offset);
}

auto GameboyColorAddressFormat::fromInteger(address_t address) const -> std::optional<Address>
{
    auto const bank = (address & 0xff0000) >> 16;
    auto const offset = address & 0x3fff;
    return AddressFormat::makeAddress(bank * 0x4000 + offset);
}

auto GameboyColorAddressFormat::read(const Binary& binary, size_t offset) const
    -> std::optional<std::pair<size_t, Address>>
{
    if (offset + 3 <= binary.size())
    {
        auto const address = binary.readAs<size_t>(offset, 3);
        return std::make_pair(offset + 3, *fromInteger(address));
    }
    else
    {
        return {};
    }
}

} // namespace fuse::systems::gbc