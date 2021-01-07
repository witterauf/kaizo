#include <contracts/Contracts.h>
#include <fuse/addresses/RegionAddressMap.h>

namespace fuse {

RegionAddressMap::RegionAddressMap(const AddressFormat* sourceFormat,
                                   const AddressFormat* targetFormat)
    : m_sourceFormat{sourceFormat}
    , m_targetFormat{targetFormat}
{
    Expects(sourceFormat);
    Expects(targetFormat);
}

static bool overlap(const Address a, size_t aSize, const Address b, size_t bSize)
{
    return !(a.applyOffset(aSize) <= b || b.applyOffset(bSize) <= a);
}

bool RegionAddressMap::isUnmapped(const Address source, size_t size) const
{
    for (auto const& mapping : m_mappings)
    {
        if (overlap(source, size, mapping.source, mapping.size))
        {
            return false;
        }
    }
    return true;
}

bool RegionAddressMap::hasSourceAddress(const Address source) const
{
    return toTargetAddress(source) ? true : false;
}

void RegionAddressMap::map(const Address source, const Address target, size_t size)
{
    Expects(source.isCompatible(*m_sourceFormat));
    Expects(target.isCompatible(*m_targetFormat));
    if (isUnmapped(source, size))
    {
        m_mappings.push_back({source, target, size});
    }
    else
    {
        throw std::runtime_error{"address " + source.toString() + " is already mapped"};
    }
}

auto RegionAddressMap::toTargetAddress(const Address source) const -> std::optional<Address>
{
    for (auto const& mapping : m_mappings)
    {
        if (mapping.source <= source && source <= mapping.source.applyOffset(mapping.size))
        {
            return mapping.target.applyOffset(source.subtract(mapping.source));
        }
    }
    return {};
}

auto RegionAddressMap::toSourceAddresses(const Address target) const -> std::vector<Address>
{
    std::vector<Address> addresses;
    for (auto const& mapping : m_mappings)
    {
        if (mapping.target <= target && target <= mapping.target.applyOffset(mapping.size))
        {
            addresses.push_back(mapping.source.applyOffset(target.subtract(mapping.target)));
        }
    }
    return addresses;
}

auto RegionAddressMap::sourceFormat() const -> const AddressFormat&
{
    Expects(m_sourceFormat);
    return *m_sourceFormat;
}

auto RegionAddressMap::targetFormat() const -> const AddressFormat&
{
    Expects(m_targetFormat);
    return *m_targetFormat;
}

auto RegionAddressMap::copy() const -> std::unique_ptr<AddressMap>
{
    auto copied = std::make_unique<RegionAddressMap>(m_sourceFormat, m_targetFormat);
    copied->m_mappings = m_mappings;
    return std::move(copied);
}

} // namespace fuse