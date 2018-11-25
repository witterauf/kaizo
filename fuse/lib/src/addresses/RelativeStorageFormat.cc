#include <diagnostics/Contracts.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/RelativeStorageFormat.h>

namespace fuse {

void RelativeStorageFormat::setBaseAddress(const Address base)
{
    Expects(base.isValid());
    m_baseAddress = base;
}

void RelativeStorageFormat::setOffsetFormat(const IntegerLayout& layout)
{
    m_layout = layout;
}

bool RelativeStorageFormat::isCompatible(const Address address) const
{
    return m_baseAddress.isCompatible(address);
}

auto RelativeStorageFormat::asLua() const -> std::string
{
    return "";
}

auto RelativeStorageFormat::writeAddress(const Address address) const -> Binary
{
    Expects(isCompatible(address));
    auto const offset = address.subtract(m_baseAddress);
    Binary binary;
    binary.append(offset, m_layout);
    return std::move(binary);
}

auto RelativeStorageFormat::writePlaceHolder() const -> Binary
{
    Binary binary;
    binary.append(0U, m_layout);
    return std::move(binary);
}

auto RelativeStorageFormat::readAddress(const Binary& binary, size_t offset) const
    -> std::optional<std::pair<size_t, Address>>
{
    auto const addressOffset = binary.readAs<AddressFormat::offset_t>(offset, m_layout);
    auto const address = m_baseAddress.applyOffset(addressOffset);
    offset += m_layout.sizeInBytes;
    return std::make_pair(offset, address);
}

} // namespace fuse