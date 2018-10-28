#pragma once

#include "Address.h"
#include "DataFormat.h"

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

private:
    std::unique_ptr<AddressFormat> m_addressFormat;
    std::unique_ptr<DataFormat> m_pointedFormat;
};

class AbsolutePointerFormat : public PointerFormat
{
protected:
    auto readAddress(DataReader& reader) -> std::optional<Address> override;
};

/*
class BaseAddressProvider
{
public:
    auto provideAddress() const -> Address;
};

class RelativePointerFormat : public PointerFormat
{
public:
private:
    std::unique_ptr<BaseAddressProvider> m_baseProvider;
    Address m_base;
};
*/

} // namespace fuse::binary