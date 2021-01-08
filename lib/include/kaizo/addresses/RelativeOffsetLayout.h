#pragma once

#include "AddressFormat.h"
#include "AddressLayout.h"
#include <kaizo/binary/Integers.h>
#include <memory>
#include <optional>

namespace kaizo {

class BaseAddressProvider
{
public:
    virtual ~BaseAddressProvider() = default;
    virtual auto provideAddress() const -> Address = 0;
    virtual auto copy() const -> std::unique_ptr<BaseAddressProvider> = 0;
};

class FixedBaseAddressProvider : public BaseAddressProvider
{
public:
    explicit FixedBaseAddressProvider(Address address);
    auto provideAddress() const -> Address override;
    auto copy() const -> std::unique_ptr<BaseAddressProvider> override;

private:
    Address m_address;
};

class RelativeOffsetLayout : public AddressLayout
{
public:
    void setBaseAddress(const Address base);
    auto baseAddress() const -> Address;
    void setOffsetFormat(const IntegerLayout& layout);
    auto offsetLayout() const -> IntegerLayout;

    struct NullPointer
    {
        // Offset that is considered NULL
        AddressFormat::offset_t offset;
        // Address this is translated to
        Address address;
    };

    void setNullPointer(const Address null, AddressFormat::offset_t offset);
    bool hasNullPointer() const;
    auto nullPointer() const -> NullPointer;

    auto getName() const -> std::string override;
    bool isCompatible(const Address address) const override;
    auto writeAddress(const Address address) const -> std::vector<BinaryPatch> override;
    auto writePlaceHolder() const -> std::vector<BinaryPatch> override;
    auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;
    auto copy() const -> std::unique_ptr<AddressLayout> override;

private:
    std::optional<NullPointer> m_nullPointer;
    Address m_baseAddress;
    IntegerLayout m_layout;
};

} // namespace kaizo