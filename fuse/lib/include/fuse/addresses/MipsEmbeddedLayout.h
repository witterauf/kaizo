#pragma once

#include "AddressFormat.h"
#include "AddressStorageFormat.h"
#include <memory>
#include <optional>

namespace fuse {

class MipsEmbeddedLayout : public AddressStorageFormat
{
public:
    static auto deserialize(LuaDomReader& reader) -> std::unique_ptr<MipsEmbeddedLayout>;

    void setBaseAddress(const Address base);
    void setOffsets(signed hi16, signed lo16);

    bool isCompatible(const Address address) const override;
    void serialize(LuaWriter& writer) const override;
    auto writeAddress(const Address address) const -> std::vector<BinaryPatch> override;
    auto writePlaceHolder() const -> std::vector<BinaryPatch> override;
    auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;
    auto copy() const -> std::unique_ptr<AddressStorageFormat> override;

private:
    signed m_offsetHi16{0}, m_offsetLo16{4};
    Address m_baseAddress;
};

} // namespace fuse