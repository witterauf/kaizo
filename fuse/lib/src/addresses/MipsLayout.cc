#include "fuse/addresses/MipsLayout.h"
#include "fuse/addresses/AbsoluteOffset.h"
#include "fuse/addresses/AddressFormat.h"
#include "fuse/utilities/NarrowCast.h"
#include <contracts/Contracts.h>

namespace fuse {

void MipsLayout::setBaseAddress(const Address base)
{
    Expects(base.isValid());
    m_baseAddress = base;
}

void MipsLayout::setOffsets(signed hi16, signed lo16)
{
    Expects(lo16 < hi16 || lo16 >= hi16 + 4);
    Expects(hi16 < lo16 || hi16 >= lo16 + 4);
    m_offsetHi16 = hi16;
    m_offsetLo16 = lo16;
}

bool MipsLayout::isCompatible(const Address address) const
{
    return m_baseAddress.isCompatible(address);
}

auto MipsLayout::writeAddress(const Address address) const -> std::vector<BinaryPatch>
{
    Expects(isCompatible(address));
    auto const offset = static_cast<uint32_t>(address.subtract(m_baseAddress));
    auto const loPart = static_cast<uint16_t>(offset & 0xFFFF);
    // The loPart is interpreted as a signed 16-bit number, so everything >= 0x8000 is actually
    // negative; therefore, we have to increase the hiPart by 1 so the address is correct
    // after dynamic relocation.
    auto const hiPart = static_cast<uint16_t>((offset >> 16) + (loPart >= 0x8000 ? 1 : 0));
    BinaryPatch hi16{hiPart, 0xFFFF, 4, m_offsetHi16};
    BinaryPatch lo16{loPart, 0xFFFF, 4, m_offsetLo16};
    if (m_offsetHi16 > m_offsetLo16)
    {
        return {lo16, hi16};
    }
    else
    {
        return {hi16, lo16};
    }
}

auto MipsLayout::writePlaceHolder() const -> std::vector<BinaryPatch>
{
    BinaryPatch hi16{0, 0xFFFF, 4, m_offsetHi16};
    BinaryPatch lo16{0, 0xFFFF, 4, m_offsetLo16};
    if (m_offsetHi16 > m_offsetLo16)
    {
        return {lo16, hi16};
    }
    else
    {
        return {hi16, lo16};
    }
}

auto MipsLayout::readAddress(const Binary& binary, size_t offset) const
    -> std::optional<std::pair<size_t, Address>>
{
    auto const hi16 = binary.readLittle<2, uint16_t>(offset + m_offsetHi16);
    auto const lo16 = binary.readLittle<2, uint16_t>(offset + m_offsetLo16);
    auto const addressOffset = (hi16 << 16) + static_cast<int16_t>(lo16);
    offset += std::max(m_offsetHi16, m_offsetLo16) + 4;
    auto const address = m_baseAddress.applyOffset(addressOffset);
    return std::make_pair(offset, address);
}

auto MipsLayout::copy() const -> std::unique_ptr<AddressLayout>
{
    auto copied = std::make_unique<MipsLayout>();
    copied->m_baseAddress = m_baseAddress;
    copied->m_offsetHi16 = m_offsetHi16;
    copied->m_offsetLo16 = m_offsetLo16;
    return copied;
}

auto MipsLayout::getName() const -> std::string
{
    return "mips";
}

} // namespace fuse