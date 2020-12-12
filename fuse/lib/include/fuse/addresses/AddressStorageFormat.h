#pragma once

#include "Address.h"
#include <cstddef>
#include <fuse/Binary.h>
#include <fuse/BinaryPatch.h>
#include <optional>
#include <string>
#include <utility>

namespace fuse {

class LuaWriter;
class LuaDomReader;

class AddressStorageFormat
{
public:
    static auto deserialize(LuaDomReader& reader) -> std::unique_ptr<AddressStorageFormat>;

    virtual ~AddressStorageFormat() = default;

    virtual auto getName() const -> std::string = 0;
    virtual bool isCompatible(const Address address) const = 0;
    virtual void serialize(LuaWriter& writer) const = 0;
    virtual auto writeAddress(const Address address) const -> std::vector<BinaryPatch> = 0;
    virtual auto writePlaceHolder() const -> std::vector<BinaryPatch> = 0;
    virtual auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressStorageFormat> = 0;
};

} // namespace fuse