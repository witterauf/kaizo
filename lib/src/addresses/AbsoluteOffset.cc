#include <kaizo/addresses/AbsoluteOffset.h>
#include <kaizo/addresses/Address.h>
#include <kaizo/utilities/StringAlgorithms.h>

namespace kaizo {

auto AbsoluteOffset::applyOffset(const Address& address, offset_t offset) const -> Address
{
    return makeAddress(address.toInteger() + offset);
}

auto AbsoluteOffset::subtract(const Address& a, const Address& b) const -> offset_t
{
    return a.toInteger() - b.toInteger();
}

auto AbsoluteOffset::fromInteger(address_t address) const -> std::optional<Address>
{
    return makeAddress(address);
}

auto AbsoluteOffset::read(const Binary&, size_t) const -> std::optional<std::pair<size_t, Address>>
{
    return {};
}

auto AbsoluteOffset::copy() const -> std::unique_ptr<AddressFormat>
{
    return std::make_unique<AbsoluteOffset>();
}

auto AbsoluteOffset::toString(const Address& address) const -> std::string
{
    return "0x" + kaizo::toString(address.toInteger(), 16, 8);
}

auto fileOffsetFormat() -> const AbsoluteOffset*
{
    static auto fileOffset = std::make_unique<AbsoluteOffset>();
    return fileOffset.get();
}

} // namespace kaizo