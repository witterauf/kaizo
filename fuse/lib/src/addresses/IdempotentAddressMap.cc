#include <contracts/Contracts.h>
#include <fuse/addresses/IdempotentAddressMap.h>

namespace fuse {

IdempotentAddressMap::IdempotentAddressMap(const AddressFormat* format)
    : m_format{format}
{
    Expects(format);
}

auto IdempotentAddressMap::sourceFormat() const -> const AddressFormat&
{
    Expects(m_format);
    return *m_format;
}

auto IdempotentAddressMap::targetFormat() const -> const AddressFormat&
{
    Expects(m_format);
    return *m_format;
}

auto IdempotentAddressMap::toTargetAddress(const Address address) const -> std::optional<Address>
{
    return address;
}

auto IdempotentAddressMap::toSourceAddresses(const Address address) const -> std::vector<Address>
{
    return {address};
}

auto IdempotentAddressMap::copy() const -> std::unique_ptr<AddressMap>
{
    return std::make_unique<IdempotentAddressMap>(m_format);
}

} // namespace fuse