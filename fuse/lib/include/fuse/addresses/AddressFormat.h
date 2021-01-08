#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace kaizo::data {

class Address;
class Binary;

class AddressFormat
{
public:
    using address_t = uint64_t;
    using offset_t = int64_t;

    virtual ~AddressFormat() = default;

    virtual auto applyOffset(const Address& address, offset_t offset) const -> Address = 0;
    virtual auto subtract(const Address& a, const Address& b) const -> offset_t = 0;
    virtual auto fromInteger(address_t address) const -> std::optional<Address> = 0;
    virtual auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressFormat> = 0;
    virtual auto toString(const Address& address) const -> std::string = 0;
    virtual bool isCompatible(const AddressFormat& format) const;

protected:
    auto makeAddress(address_t address) const -> Address;
};

} // namespace kaizo::data