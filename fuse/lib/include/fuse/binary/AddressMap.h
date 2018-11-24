#pragma once

#include <fuse/binary/Address.h>
#include <optional>
#include <vector>

namespace fuse::binary {

class AddressMap
{
public:
    virtual auto sourceFormat() const -> const AddressFormat& = 0;
    virtual auto targetFormat() const -> const AddressFormat& = 0;
    virtual auto toTargetAddress(const Address) const -> std::optional<Address> = 0;
    virtual auto toSourceAddresses(const Address) const -> std::vector<Address> = 0;
    virtual auto copy() const -> std::unique_ptr<AddressMap> = 0;
};

class IdempotentAddressMap : public AddressMap
{
public:
    explicit IdempotentAddressMap(const AddressFormat* format);

    auto sourceFormat() const -> const AddressFormat& override;
    auto targetFormat() const -> const AddressFormat& override;
    auto toTargetAddress(const Address) const -> std::optional<Address> override;
    auto toSourceAddresses(const Address) const -> std::vector<Address> override;
    auto copy() const -> std::unique_ptr<AddressMap> override;

private:
    const AddressFormat* m_format{nullptr};
};

class RegionAddressMap : public AddressMap
{
public:
    explicit RegionAddressMap(const AddressFormat* sourceFormat, const AddressFormat* targetFormat);

    void map(const Address source, const Address destination, size_t size);

    auto sourceFormat() const -> const AddressFormat& override;
    auto targetFormat() const -> const AddressFormat& override;
    bool hasSourceAddress(const Address source) const;
    auto toTargetAddress(const Address source) const -> std::optional<Address> override;
    auto toSourceAddresses(const Address source) const -> std::vector<Address> override;
    auto copy() const -> std::unique_ptr<AddressMap> override;

private:
    const AddressFormat* m_sourceFormat;
    const AddressFormat* m_targetFormat;

    bool isUnmapped(const Address source, size_t size) const;

    struct Mapping
    {
        Address source;
        Address target;
        size_t size;
    };

    std::vector<Mapping> m_mappings;
};

} // namespace fuse::binary