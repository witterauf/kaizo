#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace kaizo::data {

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

} // namespace kaizo::data