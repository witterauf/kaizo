#pragma once

#include <fuse/binary/Address.h>

namespace fuse::binary {

class AbsoluteOffset : public AddressFormat
{
public:
    auto applyOffset(const Address& address, int64_t offset) const -> Address override;
    auto subtract(const Address& a, const Address& b) const -> int64_t override;
    auto fromInteger(uint64_t address) const -> std::optional<Address> override;
    auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;
    auto copy() const -> std::unique_ptr<AddressFormat> override;
    auto toString(const Address& address) const -> std::string override;
};

auto fileOffsetFormat() -> const AbsoluteOffset*;

} // namespace fuse::binary