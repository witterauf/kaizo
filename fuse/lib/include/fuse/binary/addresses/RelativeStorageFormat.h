#pragma once

#include "AddressStorageFormat.h"
#include <fuse/binary/IntegerFormat.h>
#include <memory>

namespace fuse::binary {

class RelativeStorageFormat : public AddressStorageFormat
{
public:
    void setBaseAddress(const Address base);
    void setOffsetFormat(std::unique_ptr<IntegerFormat>&& format);

    bool isCompatible(const Address address) const override;
    auto asLua() const -> std::string override;
    auto writeAddress(const Address address) const -> Binary override;
    auto writePlaceHolder() const -> Binary override;
    auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> override;

private:
    Address m_baseAddress;
    std::unique_ptr<IntegerFormat> m_format;
};

} // namespace fuse::binary