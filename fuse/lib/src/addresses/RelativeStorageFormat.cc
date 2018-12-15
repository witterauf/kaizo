#include <diagnostics/Contracts.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/RelativeStorageFormat.h>

namespace fuse {

void RelativeStorageFormat::setBaseAddress(const Address base)
{
    Expects(base.isValid());
    m_baseAddress = base;
}

void RelativeStorageFormat::setNullPointer(const Address null, AddressFormat::offset_t offset)
{
    m_nullPointer = NullPointer{offset, null};
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
    std::string lua;
    lua += "{\n";
    lua += "  ";
    lua += "}\n";
    return lua;
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

auto RelativeStorageFormat::copy() const -> std::unique_ptr<AddressStorageFormat>
{
    auto copied = std::make_unique<RelativeStorageFormat>();
    copied->m_nullPointer = m_nullPointer;
    copied->m_baseAddress = m_baseAddress;
    copied->m_layout = m_layout;
    return std::move(copied);
}

} // namespace fuse