#pragma once

#include "Address.h"
#include <optional>
#include <vector>

namespace fuse {

class AddressMap
{
public:
    virtual auto sourceFormat() const -> const AddressFormat& = 0;
    virtual auto targetFormat() const -> const AddressFormat& = 0;
    virtual auto toTargetAddress(const Address) const -> std::optional<Address> = 0;
    virtual auto toSourceAddresses(const Address) const -> std::vector<Address> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressMap> = 0;
};

} // namespace fuse