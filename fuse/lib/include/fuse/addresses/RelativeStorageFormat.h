#pragma once

#include "AddressFormat.h"
#include "AddressStorageFormat.h"
#include <fuse/Integers.h>
#include <memory>
#include <optional>

namespace fuse {

class BaseAddressProvider
{
public:
    virtual auto provideAddress() const -> Address = 0;
    virtual auto copy() const -> std::unique_ptr<BaseAddressProvider> = 0;
    virtual void serialize(LuaWriter& writer) const = 0;
};

class FixedBaseAddressProvider : public BaseAddressProvider
{
public:
    explicit FixedBaseAddressProvider(Address address);
    auto provideAddress() const -> Address override;
    auto copy() const -> std::unique_ptr<BaseAddressProvider> override;
    void serialize(LuaWriter& writer) const override;

private:
    Address m_address;
};

class RelativeStorageFormat : public AddressStorageFormat
{
public:
    static auto deserialize(LuaDomReader& reader) -> std::unique_ptr<RelativeStorageFormat>;

    void setBaseAddress(const Address base);
    void setNullPointer(const Address null, AddressFormat::offset_t offset);
    void setOffsetFormat(const IntegerLayout& layout);

    bool isCompatible(const Address address) const override;
    void serialize(LuaWriter& writer) const override;
    auto writeAddress(const Address address) const -> std::vector<BinaryPatch> override;
    auto writePlaceHolder() const -> std::vector<BinaryPatch> override;
    auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;
    auto copy() const -> std::unique_ptr<AddressStorageFormat> override;

private:
    struct NullPointer
    {
        AddressFormat::offset_t offset;
        Address address;
    };
    std::optional<NullPointer> m_nullPointer;
    Address m_baseAddress;
    IntegerLayout m_layout;
};

} // namespace fuse