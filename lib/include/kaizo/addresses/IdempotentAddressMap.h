#pragma once

#include "AddressMap.h"

namespace kaizo {

class IdempotentAddressMap : public AddressMap
{
public:
    explicit IdempotentAddressMap(const AddressFormat* format);

    auto sourceFormat() const -> const AddressFormat& override;
    auto targetFormat() const -> const AddressFormat& override;
    auto toTargetAddress(const Address) const -> std::optional<Address> override;
    auto toSourceAddresses(const Address) const -> std::vector<Address> override;
    auto copy() const -> std::unique_ptr<AddressMap> override;

private:
    const AddressFormat* m_format{nullptr};
};

} // namespace kaizo