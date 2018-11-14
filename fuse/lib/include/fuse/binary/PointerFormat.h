#pragma once

#include "Address.h"
#include "DataFormat.h"
#include "IntegerFormat.h"

namespace fuse::binary {

class PointerFormat : public DataFormat
{
public:
    void setAddressFormat(std::unique_ptr<AddressFormat>&& format);
    void setPointedFormat(std::unique_ptr<DataFormat>&& format);
    auto addressFormat() const -> const AddressFormat&;

protected:
    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    virtual auto readAddress(DataReader& reader) -> std::optional<Address> = 0;
    void copyPointerFormat(PointerFormat& format) const;

private:
    std::unique_ptr<AddressFormat> m_addressFormat;
    std::unique_ptr<DataFormat> m_pointedFormat;
};

class AbsolutePointerFormat : public PointerFormat
{
protected:
    auto readAddress(DataReader& reader) -> std::optional<Address> override;
};

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
    void setBaseAddressProvider(std::unique_ptr<BaseAddressProvider>&& provider);
    void setOffsetFormat(std::unique_ptr<IntegerFormat>&& offsetFormat);
    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    auto readAddress(DataReader& reader) -> std::optional<Address> override;

private:
    std::unique_ptr<IntegerFormat> m_offsetFormat;
    std::unique_ptr<BaseAddressProvider> m_baseProvider;
};

} // namespace fuse::binary