#include <diagnostics/Contracts.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/NullData.h>
#include <fuse/binary/data/ReferenceData.h>
#include <fuse/binary/formats/PointerFormat.h>

namespace fuse::binary {

void PointerFormat::useAddressMap(bool on)
{
    m_useAddressMap = on;
}

void PointerFormat::setNullPointer(const Address& null)
{
    m_nullPointer = null;
}

void PointerFormat::setAddressFormat(AddressFormat* format)
{
    Expects(format);
    m_addressFormat = format;
}

void PointerFormat::setPointedFormat(std::unique_ptr<DataFormat>&& format)
{
    Expects(format);
    m_pointedFormat = std::move(format);
}

auto PointerFormat::addressFormat() const -> const AddressFormat&
{
    Expects(m_addressFormat);
    return *m_addressFormat;
}

auto PointerFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    Expects(m_addressFormat);
    Expects(m_pointedFormat);

    if (auto maybeAddress = readAddress(reader))
    {
        if (!m_nullPointer || !(*maybeAddress == *m_nullPointer))
        {
            auto const oldOffset = reader.offset();
            uint64_t newOffset;
            if (m_useAddressMap)
            {
                auto const addresses = reader.addressMap().toSourceAddresses(*maybeAddress);
                if (addresses.size() == 1)
                {
                    newOffset = addresses.front().toInteger();
                }
                else if (addresses.empty())
                {
                    throw std::runtime_error{"could not map address"};
                }
                else
                {
                    throw std::runtime_error{"address maps to more than one source address"};
                }
            }
            else
            {
                newOffset = maybeAddress->toInteger();
            }

            reader.setOffset(newOffset);
            if (auto data = m_pointedFormat->decode(reader))
            {
                reader.setOffset(oldOffset);
                return std::move(data);
            }
        }
        else
        {
            return std::make_unique<NullData>();
        }
    }
    return {};
}

void PointerFormat::doEncode(DataWriter& writer, const Data& data)
{
    Expects(m_addressFormat);
    DataPath destination;

    if (data.type() == DataType::Reference)
    {
        auto const referenceData = static_cast<const ReferenceData&>(data);
        destination = referenceData.path();
    }
    else if (m_pointedFormat)
    {
        writer.enterLevel();
        writer.enter(DataPathElement::makePointer());
        m_pointedFormat->encode(writer, data);
        destination = writer.path();
        writer.leave();
        writer.leaveLevel();
    }
    else
    {
        throw std::runtime_error{"reference data or pointed format required at '" +
                                 writer.path().toString() + "'"};
    }

    writer.addUnresolvedReference(makeStorageFormat(), destination);
    writeAddressPlaceHolder(writer);
}

void PointerFormat::copyPointerFormat(PointerFormat& format) const
{
    format.m_addressFormat = m_addressFormat;
    if (m_pointedFormat)
    {
        format.m_pointedFormat = m_pointedFormat->copy();
    }
    format.m_useAddressMap = m_useAddressMap;
    format.m_nullPointer = m_nullPointer;
    copyDataFormat(format);
}

bool PointerFormat::hasNullPointer() const
{
    return m_nullPointer.has_value();
}

auto PointerFormat::nullPointer() const -> const Address&
{
    Expects(hasNullPointer());
    return *m_nullPointer;
}

} // namespace fuse::binary