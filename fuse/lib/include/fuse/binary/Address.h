#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace fuse {
class Binary;
}

namespace fuse::binary {

class Address;

class AddressFormat
{
public:
    virtual ~AddressFormat() = default;
    virtual auto applyOffset(const Address& address, int64_t offset) const -> Address = 0;
    virtual auto subtract(const Address& a, const Address& b) const -> int64_t = 0;
    virtual auto fromInteger(uint64_t address) const -> std::optional<Address> = 0;
    virtual auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressFormat> = 0;
    virtual auto toString(const Address& address) const -> std::string = 0;
    virtual bool isCompatible(const AddressFormat& format) const;

protected:
    auto makeAddress(size_t address) const -> Address;
};

class Address
{
    friend class AddressFormat;

public:
    Address() = default;

    bool isValid() const;
    auto toInteger() const -> uint64_t;
    auto applyOffset(int64_t offset) const -> Address;
    bool isCompatible(const AddressFormat& format) const;
    bool isCompatible(const Address& address) const;
    bool isLessThan(const Address& rhs) const;
    bool isEqual(const Address& rhs) const;
    auto subtract(const Address& rhs) const -> int64_t;
    auto toString() const -> std::string;

    bool operator==(const Address& rhs) const;
    bool operator<(const Address& rhs) const;
    bool operator<=(const Address& rhs) const;

private:
    explicit Address(size_t address, const AddressFormat* format);

    uint64_t m_address{0};
    const AddressFormat* m_format{nullptr};
};

} // namespace fuse::binary