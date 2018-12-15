#include <diagnostics/Contracts.h>
#include <fuse/Binary.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/UnresolvedReference.h>
#include <fuse/binary/formats/RelativePointerFormat.h>

namespace fuse::binary {

void RelativeOffsetFormat::setNullPointerOffset(AddressFormat::offset_t offset)
{
    Expects(hasNullPointer());
    m_offsetFormat->setNullPointer(nullPointer(), offset);
}

auto RelativeOffsetFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    Expects(m_offsetFormat);
    Expects(m_baseProvider);

    auto const baseAddress = m_baseProvider->provideAddress();
    m_offsetFormat->setBaseAddress(baseAddress);

    if (auto maybeOffsetAddress = m_offsetFormat->readAddress(reader.binary(), reader.offset()))
    {
        reader.setOffset(maybeOffsetAddress->first);
        return maybeOffsetAddress->second;
    }
    return {};
}

void RelativeOffsetFormat::setBaseAddressProvider(std::unique_ptr<BaseAddressProvider>&& provider)
{
    m_baseProvider = std::move(provider);
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
        auto copied = m_offsetFormat->copy();
        format->m_offsetFormat = std::unique_ptr<RelativeStorageFormat>(
            static_cast<RelativeStorageFormat*>(copied.release()));
    }
    format->m_nullPointerOffset = m_nullPointerOffset;
    copyPointerFormat(*format);
    return std::move(format);
}

void RelativeOffsetFormat::setOffsetFormat(std::unique_ptr<RelativeStorageFormat>&& offsetFormat)
{
    m_offsetFormat = std::move(offsetFormat);
    auto copied = m_offsetFormat->copy();
    m_referenceFormat = std::shared_ptr<AddressStorageFormat>(copied.release());
}

auto RelativeOffsetFormat::makeStorageFormat() -> std::shared_ptr<AddressStorageFormat>
{
    return m_referenceFormat;
}

void RelativeOffsetFormat::writeAddressPlaceHolder(DataWriter& writer)
{
    writer.binary().append(m_offsetFormat->writePlaceHolder());
}

} // namespace fuse::binary