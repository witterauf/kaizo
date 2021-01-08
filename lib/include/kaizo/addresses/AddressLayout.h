#pragma once

#include "Address.h"
#include <cstddef>
#include <kaizo/binary/Binary.h>
#include <kaizo/binary/BinaryPatch.h>
#include <optional>
#include <string>
#include <utility>

namespace kaizo::data {

class AddressLayout
{
public:
    virtual ~AddressLayout() = default;

    virtual auto getName() const -> std::string = 0;
    virtual bool isCompatible(const Address address) const = 0;
    virtual auto writeAddress(const Address address) const -> std::vector<BinaryPatch> = 0;
    virtual auto writePlaceHolder() const -> std::vector<BinaryPatch> = 0;
    virtual auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressLayout> = 0;
};

} // namespace kaizo::data