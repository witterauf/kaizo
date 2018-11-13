#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
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
    virtual auto delinearize(size_t address) const -> std::optional<Address> = 0;
    virtual auto read(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressFormat> = 0;

protected:
    auto makeAddress(size_t address) const -> Address;
};

class Address
{
    friend class AddressFormat;

public:
    auto linearize() const -> size_t;
    auto applyOffset(int64_t offset) const -> Address;

private:
    explicit Address(size_t address, const AddressFormat* format);

    size_t m_address;
    const AddressFormat* m_format{nullptr};
};

} // namespace fuse::binary