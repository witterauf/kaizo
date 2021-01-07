#include <diagnostics/Contracts.h>
#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/addresses/RelativeStorageFormat.h>
#include <fuse/utilities/DomReaderHelpers.h>

namespace fuse {

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

/*
void FixedBaseAddressProvider::serialize(LuaWriter& writer) const
{
    writer.writeInteger(m_address.toInteger());
}

auto RelativeStorageFormat::deserialize(LuaDomReader& reader)
    -> std::unique_ptr<RelativeStorageFormat>
{
    Expects(reader.isRecord());
    auto format = std::make_unique<RelativeStorageFormat>();
    auto const baseOffset = requireUnsignedInteger(reader, "base");
    enterRecord(reader, "layout");
    auto const layout = IntegerLayout::deserialize(reader);
    reader.leave();
    format->setOffsetFormat(layout);
    if (auto maybeAddress = fileOffsetFormat()->fromInteger(baseOffset))
    {
        format->setBaseAddress(*maybeAddress);
    }
    else
    {
        throw FuseException{"could not deserialize RelativeStorageFormat"};
    }
    return format;
}

void RelativeStorageFormat::serialize(LuaWriter& writer) const
{
    writer.startTable();
    writer.startField("class").writeString("RelativeOffset").finishField();
    if (m_baseAddress.toInteger() != 0)
    {
        writer.startField("base").writeInteger(m_baseAddress.toInteger()).finishField();
    }
    if (m_nullPointer)
    {
        writer.startField("null_pointer").startTable();
        writer.startField("offset").writeInteger(m_nullPointer->offset).finishField();
        writer.startField("address").writeInteger(m_nullPointer->address.toInteger()).finishField();
        writer.finishTable().finishField();
    }
    writer.startField("layout");
    fuse::serialize(writer, m_layout);
    writer.finishField();
    writer.finishTable();
}

*/

void RelativeStorageFormat::setBaseAddress(const Address base)
{
    Expects(base.isValid());
    m_baseAddress = base;
}

auto RelativeStorageFormat::baseAddress() const -> Address
{
    return m_baseAddress;
}

void RelativeStorageFormat::setNullPointer(const Address null, AddressFormat::offset_t offset)
{
    m_nullPointer = NullPointer{offset, null};
}

bool RelativeStorageFormat::hasNullPointer() const
{
    return m_nullPointer.has_value();
}

auto RelativeStorageFormat::nullPointer() const -> NullPointer
{
    return *m_nullPointer;
}

void RelativeStorageFormat::setOffsetFormat(const IntegerLayout& layout)
{
    m_layout = layout;
}

auto RelativeStorageFormat::offsetLayout() const -> IntegerLayout
{
    return m_layout;
}

bool RelativeStorageFormat::isCompatible(const Address address) const
{
    return m_baseAddress.isCompatible(address);
}

auto RelativeStorageFormat::writeAddress(const Address address) const -> std::vector<BinaryPatch>
{
    Expects(isCompatible(address));
    auto const offset = address.subtract(m_baseAddress);
    Binary binary;
    binary.append(offset, m_layout);
    return {BinaryPatch{binary}};
}

auto RelativeStorageFormat::writePlaceHolder() const -> std::vector<BinaryPatch>
{
    Binary binary;
    binary.append(0U, m_layout);
    return {BinaryPatch{binary}};
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

auto RelativeStorageFormat::getName() const -> std::string
{
    return "relative";
}

} // namespace fuse