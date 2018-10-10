#pragma once

#include "DataFormat.h"

namespace fuse::binary {

class AddressFormat
{
public:
    virtual ~AddressFormat() = default;
    virtual auto applyOffset(const Address& address, int64_t offset) const -> Address;
    virtual auto delinearize(size_t address) const -> Address;
};

class Address
{
public:
    auto linearize() const -> size_t;
    auto applyOffset(int64_t offset) const -> Address;

private:
    size_t m_address;
    const AddressFormat* m_format;
};

class PointerFormat : public DataFormat
{
public:
    virtual auto readAddress(const DataReader& reader) -> Address = 0;

    auto addressFormat() const -> const AddressFormat&;

private:
    std::unique_ptr<AddressFormat> m_addressFormat;
    std::unique_ptr<DataFormat> m_pointedFormat;
};

class AbsolutePointerFormat : public PointerFormat
{
};

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

} // namespace fuse::binary