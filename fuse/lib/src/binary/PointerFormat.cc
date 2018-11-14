#include <diagnostics/Contracts.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/IntegerData.h>
#include <fuse/binary/PointerFormat.h>

namespace fuse::binary {

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
        auto const oldOffset = reader.offset();
        // replace by AddressMap?
        reader.setOffset(maybeAddress->linearize());
        if (auto data = m_pointedFormat->decode(reader))
        {
            reader.setOffset(oldOffset);
            return std::move(data);
        }
    }
    return {};
}

void PointerFormat::copyPointerFormat(PointerFormat& format) const
{
    format.m_addressFormat = m_addressFormat->copy();
    format.m_pointedFormat = m_pointedFormat->copy();
    copyDataFormat(format);
}

auto AbsolutePointerFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    if (auto maybeResult = addressFormat().read(reader.binary(), reader.offset()))
    {
        auto [newOffset, address] = *maybeResult;
        reader.setOffset(newOffset);
        return address;
    }
    return {};
}

auto RelativeOffsetFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    Expects(m_offsetFormat);
    Expects(m_baseProvider);
    if (auto maybeOffset = m_offsetFormat->decode(reader))
    {
        auto baseAddress = m_baseProvider->provideAddress();
        auto const& offset = static_cast<const IntegerData&>(*maybeOffset);
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
    return {};
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
    format->m_baseProvider = m_baseProvider->copy();
    format->m_offsetFormat = m_offsetFormat->copyAs<IntegerFormat>();
    copyPointerFormat(*format);
    return std::move(format);
}

} // namespace fuse::binary