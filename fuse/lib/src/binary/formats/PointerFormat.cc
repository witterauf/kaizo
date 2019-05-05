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

auto PointerFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto format = std::make_unique<PointerFormat>();
    if (m_layout)
    {
        format->setLayout(m_layout->copy());
    }
    copyPointerFormat(*format);
    return format;
}

void PointerFormat::setLayout(std::unique_ptr<AddressStorageFormat>&& layout)
{
    m_layout = std::shared_ptr<AddressStorageFormat>(layout.release());
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

auto PointerFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    Expects(m_layout);

    if (auto maybeResult = m_layout->readAddress(reader.binary(), reader.offset()))
    {
        auto const [newOffset, address] = *maybeResult;
        reader.setOffset(newOffset);
        return address;
    }
    return {};
}

void PointerFormat::writeAddressPlaceHolder(DataWriter& writer)
{
    Expects(m_layout);
    auto const patches = m_layout->writePlaceHolder();
    size_t offset{0};
    // assume patches are ordered according to relativeOffset
    for (auto const& patch : patches)
    {
        if (patch.relativeOffset() < 0)
        {
            throw std::runtime_error{"address placeholder: relative offset must be positive"};
        }
        if (static_cast<size_t>(patch.relativeOffset()) > offset)
        {
            writer.skip(patch.relativeOffset() - offset);
            offset = patch.relativeOffset();
        }
        writer.binary().append(Binary{patch.size()});
        offset += patch.size();
    }
}

auto PointerFormat::makeStorageFormat() -> std::shared_ptr<AddressStorageFormat>
{
    Expects(m_layout);
    return m_layout;
}

} // namespace fuse::binary