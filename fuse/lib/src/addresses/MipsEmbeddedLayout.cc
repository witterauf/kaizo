#include <diagnostics/Contracts.h>
#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/MipsEmbeddedLayout.h>
#include <fuse/lua/LuaReader.h>
#include <fuse/lua/LuaWriter.h>
#include <fuse/utilities/DomReaderHelpers.h>
#include <fuse/utilities/NarrowCast.h>

namespace fuse {

auto MipsEmbeddedLayout::deserialize(LuaDomReader& reader) -> std::unique_ptr<MipsEmbeddedLayout>
{
    Expects(reader.isRecord());
    auto format = std::make_unique<MipsEmbeddedLayout>();
    auto const baseOffset = requireUnsignedInteger(reader, "base");
    format->m_offsetHi16 = narrow<signed>(requireSignedInteger(reader, "offset_hi16"));
    format->m_offsetLo16 = narrow<signed>(requireSignedInteger(reader, "offset_lo16"));
    if (auto maybeAddress = fileOffsetFormat()->fromInteger(baseOffset))
    {
        format->setBaseAddress(*maybeAddress);
    }
    else
    {
        throw FuseException{"could not deserialize MipsEmbeddedLayout"};
    }
    return std::move(format);
}

void MipsEmbeddedLayout::setBaseAddress(const Address base)
{
    Expects(base.isValid());
    m_baseAddress = base;
}

void MipsEmbeddedLayout::setOffsets(signed hi16, signed lo16)
{
    Expects(lo16 < hi16 || lo16 >= hi16 + 4);
    Expects(hi16 < lo16 || hi16 >= lo16 + 4);
    m_offsetHi16 = hi16;
    m_offsetLo16 = lo16;
}

bool MipsEmbeddedLayout::isCompatible(const Address address) const
{
    return m_baseAddress.isCompatible(address);
}

void MipsEmbeddedLayout::serialize(LuaWriter& writer) const
{
    writer.startTable();
    writer.startField("class").writeString("MipsEmbeddedHiLo").finishField();
    if (m_baseAddress.toInteger() != 0)
    {
        writer.startField("base").writeInteger(m_baseAddress.toInteger()).finishField();
    }
    writer.startField("offset_hi16").writeInteger(static_cast<int64_t>(m_offsetHi16)).finishField();
    writer.startField("offset_lo16").writeInteger(static_cast<int64_t>(m_offsetLo16)).finishField();
    writer.finishTable();
}

auto MipsEmbeddedLayout::writeAddress(const Address address) const -> std::vector<BinaryPatch>
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

auto MipsEmbeddedLayout::writePlaceHolder() const -> std::vector<BinaryPatch>
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

auto MipsEmbeddedLayout::readAddress(const Binary& binary, size_t offset) const
    -> std::optional<std::pair<size_t, Address>>
{
    auto const hi16 = binary.readLittle<2, uint16_t>(offset + m_offsetHi16);
    auto const lo16 = binary.readLittle<2, uint16_t>(offset + m_offsetLo16);
    auto const addressOffset = (hi16 << 16) + static_cast<int16_t>(lo16);
    offset += std::max(m_offsetHi16, m_offsetLo16) + 4;
    auto const address = m_baseAddress.applyOffset(addressOffset);
    return std::make_pair(offset, address);
}

auto MipsEmbeddedLayout::copy() const -> std::unique_ptr<AddressStorageFormat>
{
    auto copied = std::make_unique<MipsEmbeddedLayout>();
    copied->m_baseAddress = m_baseAddress;
    copied->m_offsetHi16 = m_offsetHi16;
    copied->m_offsetLo16 = m_offsetLo16;
    return copied;
}

auto MipsEmbeddedLayout::getName() const -> std::string
{
    return "mips";
}

} // namespace fuse