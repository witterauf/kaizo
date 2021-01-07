#include <contracts/Contracts.h>
#include <fuse/addresses/Address.h>
#include <fuse/addresses/AddressFormat.h>

namespace fuse {

Address::Address(size_t address, const AddressFormat* format)
    : m_address{address}
    , m_format{format}
{
}

auto Address::toInteger() const -> size_t
{
    return m_address;
}

auto Address::applyOffset(int64_t offset) const -> Address
{
    Expects(m_format);
    return m_format->applyOffset(*this, offset);
}

bool Address::isCompatible(const AddressFormat& format) const
{
    return m_format == &format;
}

bool Address::isCompatible(const Address& address) const
{
    Expects(m_format);
    return m_format->isCompatible(*address.m_format);
}

bool Address::isLessThan(const Address& rhs) const
{
    Expects(isCompatible(rhs));
    return m_address < rhs.m_address;
}

bool Address::isEqual(const Address& rhs) const
{
    Expects(isCompatible(rhs));
    return m_address == rhs.m_address;
}

auto Address::subtract(const Address& rhs) const -> int64_t
{
    Expects(m_format);
    Expects(isCompatible(rhs));
    return m_format->subtract(*this, rhs);
}

bool Address::isValid() const
{
    return m_format != nullptr;
}

auto Address::toString() const -> std::string
{
    if (m_format)
    {
        return m_format->toString(*this);
    }
    else
    {
        return "(invalid address)";
    }
}

bool Address::operator==(const Address& rhs) const
{
    return isEqual(rhs);
}

bool Address::operator<(const Address& rhs) const
{
    return isLessThan(rhs);
}

bool Address::operator<=(const Address& rhs) const
{
    return isEqual(rhs) || isLessThan(rhs);
}

} // namespace fuse