#include <diagnostics/Contracts.h>
#include <fuse/binary/Address.h>

namespace fuse::binary {

auto AddressFormat::makeAddress(size_t address) const -> Address
{
    return Address{address, this};
}

Address::Address(size_t address, const AddressFormat* format)
    : m_address{address}
    , m_format{format}
{
}

auto Address::linearize() const -> size_t
{
    return m_address;
}

auto Address::applyOffset(int64_t offset) const -> Address
{
    Expects(m_format);
    return m_format->applyOffset(*this, offset);
}

} // namespace fuse::binary