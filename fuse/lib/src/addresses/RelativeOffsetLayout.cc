#include "fuse/addresses/RelativeOffsetLayout.h"
#include "fuse/addresses/AbsoluteOffset.h"
#include "fuse/addresses/AddressFormat.h"
#include <contracts/Contracts.h>

namespace kaizo::data {

FixedBaseAddressProvider::FixedBaseAddressProvider(Address address)
    : m_address{address}
{
}

auto FixedBaseAddressProvider::provideAddress() const -> Address
{
    return m_address;
}

auto FixedBaseAddressProvider::copy() const -> std::unique_ptr<BaseAddressProvider>
{
    return std::make_unique<FixedBaseAddressProvider>(m_address);
}

void RelativeOffsetLayout::setBaseAddress(const Address base)
{
    Expects(base.isValid());
    m_baseAddress = base;
}

auto RelativeOffsetLayout::baseAddress() const -> Address
{
    return m_baseAddress;
}

void RelativeOffsetLayout::setNullPointer(const Address null, AddressFormat::offset_t offset)
{
    m_nullPointer = NullPointer{offset, null};
}

bool RelativeOffsetLayout::hasNullPointer() const
{
    return m_nullPointer.has_value();
}

auto RelativeOffsetLayout::nullPointer() const -> NullPointer
{
    return *m_nullPointer;
}

void RelativeOffsetLayout::setOffsetFormat(const IntegerLayout& layout)
{
    m_layout = layout;
}

auto RelativeOffsetLayout::offsetLayout() const -> IntegerLayout
{
    return m_layout;
}

bool RelativeOffsetLayout::isCompatible(const Address address) const
{
    return m_baseAddress.isCompatible(address);
}

auto RelativeOffsetLayout::writeAddress(const Address address) const -> std::vector<BinaryPatch>
{
    Expects(isCompatible(address));
    auto const offset = address.subtract(m_baseAddress);
    Binary binary;
    binary.append(offset, m_layout);
    return {BinaryPatch{binary}};
}

auto RelativeOffsetLayout::writePlaceHolder() const -> std::vector<BinaryPatch>
{
    Binary binary;
    binary.append(0U, m_layout);
    return {BinaryPatch{binary}};
}

auto RelativeOffsetLayout::readAddress(const Binary& binary, size_t offset) const
    -> std::optional<std::pair<size_t, Address>>
{
    Expects(!m_nullPointer || m_nullPointer->address.isCompatible(m_baseAddress));

    auto const addressOffset = binary.readAs<AddressFormat::offset_t>(offset, m_layout);
    offset += m_layout.sizeInBytes;
    if (m_nullPointer && addressOffset == m_nullPointer->offset)
    {
        return std::make_pair(offset, m_nullPointer->address);
    }
    else
    {
        auto const address = m_baseAddress.applyOffset(addressOffset);
        return std::make_pair(offset, address);
    }
}

auto RelativeOffsetLayout::copy() const -> std::unique_ptr<AddressLayout>
{
    auto copied = std::make_unique<RelativeOffsetLayout>();
    copied->m_nullPointer = m_nullPointer;
    copied->m_baseAddress = m_baseAddress;
    copied->m_layout = m_layout;
    return std::move(copied);
}

auto RelativeOffsetLayout::getName() const -> std::string
{
    return "relative";
}

} // namespace kaizo::data