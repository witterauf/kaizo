#pragma once

#include "PointerFormat.h"
#include <fuse/addresses/RelativeStorageFormat.h>
#include <memory>

namespace fuse::binary {

class BaseAddressProvider
{
public:
    virtual auto provideAddress() const -> Address = 0;
    virtual auto copy() const -> std::unique_ptr<BaseAddressProvider> = 0;
};

class FixedBaseAddressProvider : public BaseAddressProvider
{
public:
    explicit FixedBaseAddressProvider(Address address)
        : m_address{address}
    {
    }

    auto provideAddress() const -> Address override
    {
        return m_address;
    }

    auto copy() const -> std::unique_ptr<BaseAddressProvider> override
    {
        return std::make_unique<FixedBaseAddressProvider>(m_address);
    }

private:
    Address m_address;
};

class RelativeOffsetFormat : public PointerFormat
{
public:
    void setNullPointerOffset(AddressFormat::offset_t offset);
    void setBaseAddressProvider(std::unique_ptr<BaseAddressProvider>&& provider);
    void setOffsetFormat(std::unique_ptr<RelativeStorageFormat>&& offsetFormat);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto readAddress(DataReader& reader) -> std::optional<Address> override;
    void writeAddressPlaceHolder(DataWriter&) override{};

private:
    AddressFormat::offset_t m_nullPointerOffset;
    std::unique_ptr<BaseAddressProvider> m_baseProvider;
    std::unique_ptr<RelativeStorageFormat> m_offsetFormat;
};

} // namespace fuse::binary