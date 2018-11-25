#include <diagnostics/Contracts.h>
#include <fuse/addresses/AddressFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/data/Data.h>
#include <fuse/binary/data/IntegerData.h>
#include <fuse/binary/data/NullData.h>
#include <fuse/binary/formats/PointerFormat.h>

namespace fuse::binary {

void PointerFormat::useAddressMap(bool on)
{
    m_useAddressMap = on;
}

void PointerFormat::setAddressFormat(std::unique_ptr<AddressFormat>&& format)
{
    Expects(format);
    m_addressFormat = std::move(format);
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
        if (maybeAddress->address)
        {
            auto const oldOffset = reader.offset();
            uint64_t newOffset;
            if (m_useAddressMap)
            {
                auto const addresses =
                    reader.addressMap().toSourceAddresses(*maybeAddress->address);
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
                newOffset = maybeAddress->address->toInteger();
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
    Expects(m_pointedFormat);

    if (m_pointedFormat->isPointer())
    {
        /*
        writer.enter(DataPath::Pointer...)
        writer.placeHere();
        */
        throw std::runtime_error{"writing pointer to pointer not yet supported"};
    }
    else
    {
        writeAddressPlaceHolder(writer);
        writer.enterLevel();
        writer.enter(DataPathElement::makePointer());
        m_pointedFormat->encode(writer, data);
        writer.leave();
        writer.leaveLevel();
    }
}

void PointerFormat::copyPointerFormat(PointerFormat& format) const
{
    format.m_addressFormat = m_addressFormat->copy();
    format.m_pointedFormat = m_pointedFormat->copy();
    format.m_useAddressMap = m_useAddressMap;
    copyDataFormat(format);
}

auto AbsolutePointerFormat::readAddress(DataReader& reader) -> std::optional<AddressDescriptor>
{
    if (auto maybeResult = addressFormat().read(reader.binary(), reader.offset()))
    {
        auto [newOffset, address] = *maybeResult;
        reader.setOffset(newOffset);
        return address;
    }
    return {};
}

auto RelativeOffsetFormat::readAddress(DataReader& reader) -> std::optional<AddressDescriptor>
{
    Expects(m_offsetFormat);
    Expects(m_baseProvider);
    if (auto maybeOffset = m_offsetFormat->decode(reader))
    {
        auto const& offset = static_cast<const IntegerData&>(*maybeOffset);
        if (!m_validator || m_validator->isValid(offset))
        {
            auto baseAddress = m_baseProvider->provideAddress();
            if (m_offsetFormat->isSigned())
            {
                baseAddress = baseAddress.applyOffset(offset.asSigned());
            }
            else
            {
                baseAddress = baseAddress.applyOffset(offset.asUnsigned());
            }
            return baseAddress;
        }
        else
        {
            return AddressDescriptor::ignoreAddress();
        }
    }
    return {};
}

void RelativeOffsetFormat::setOffsetValidator(std::unique_ptr<OffsetValidator>&& validator)
{
    m_validator = std::move(validator);
}

void RelativeOffsetFormat::setBaseAddressProvider(std::unique_ptr<BaseAddressProvider>&& provider)
{
    m_baseProvider = std::move(provider);
}

void RelativeOffsetFormat::setOffsetFormat(std::unique_ptr<IntegerFormat>&& offsetFormat)
{
    m_offsetFormat = std::move(offsetFormat);
}

auto RelativeOffsetFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto format = std::make_unique<RelativeOffsetFormat>();
    if (m_baseProvider)
    {
        format->m_baseProvider = m_baseProvider->copy();
    }
    if (m_offsetFormat)
    {
        format->m_offsetFormat = m_offsetFormat->copyAs<IntegerFormat>();
    }
    if (m_validator)
    {
        format->m_validator = m_validator->copy();
    }
    copyPointerFormat(*format);
    return std::move(format);
}

} // namespace fuse::binary