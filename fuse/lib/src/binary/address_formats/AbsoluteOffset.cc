#include <fuse/binary/address_formats/AbsoluteOffset.h>

namespace fuse::binary {

auto AbsoluteOffset::applyOffset(const Address& address, int64_t offset) const -> Address
{
    return makeAddress(address.linearize() + offset);
}

auto AbsoluteOffset::delinearize(size_t address) const -> std::optional<Address>
{
    return makeAddress(address);
}

auto AbsoluteOffset::read(const Binary&, size_t) const
    -> std::optional<std::pair<size_t, Address>>
{
    return {};
}

auto AbsoluteOffset::copy() const -> std::unique_ptr<AddressFormat>
{
    return std::make_unique<AbsoluteOffset>();
}

} // namespace fuse::binary