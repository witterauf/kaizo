#pragma once

#include "AddressFormat.h"

namespace kaizo::data {

class AbsoluteOffset : public AddressFormat
{
public:
    auto applyOffset(const Address& address, offset_t offset) const -> Address override;
    auto subtract(const Address& a, const Address& b) const -> offset_t override;
    auto fromInteger(address_t address) const -> std::optional<Address> override;
    auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;
    auto copy() const -> std::unique_ptr<AddressFormat> override;
    auto toString(const Address& address) const -> std::string override;
};

auto fileOffsetFormat() -> const AbsoluteOffset*;

} // namespace kaizo::data::binary