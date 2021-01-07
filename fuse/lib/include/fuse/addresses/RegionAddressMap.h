#pragma once

#include "AddressMap.h"

namespace kaizo::data {

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

} // namespace kaizo::data