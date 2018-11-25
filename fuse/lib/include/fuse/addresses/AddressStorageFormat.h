#pragma once

#include "Address.h"
#include <cstddef>
#include <fuse/Binary.h>
#include <optional>
#include <string>
#include <utility>

namespace fuse {

class AddressStorageFormat
{
public:
    virtual bool isCompatible(const Address address) const = 0;
    virtual auto asLua() const -> std::string = 0;
    virtual auto writeAddress(const Address address) const -> Binary = 0;
    virtual auto writePlaceHolder() const -> Binary = 0;
    virtual auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
};

} // namespace fuse