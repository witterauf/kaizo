#include <diagnostics/Contracts.h>
#include <fuse/Binary.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/formats/RelativePointerFormat.h>
#include <fuse/binary/objects/UnresolvedReference.h>

namespace fuse::binary {
    /*
auto RelativeOffsetFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    Expects(m_offsetFormat);

    if (auto maybeOffsetAddress = m_offsetFormat->readAddress(reader.binary(), reader.offset()))
    {
        reader.setOffset(maybeOffsetAddress->first);
        return maybeOffsetAddress->second;
    }
    return {};
}

auto RelativeOffsetFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto format = std::make_unique<RelativeOffsetFormat>();
    if (m_offsetFormat)
    {
        auto copied = m_offsetFormat->copy();
        format->m_offsetFormat = std::unique_ptr<RelativeStorageFormat>(
            static_cast<RelativeStorageFormat*>(copied.release()));
    }
    format->m_referenceFormat = m_referenceFormat;
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
    Expects(m_referenceFormat);
    return m_referenceFormat;
}

void RelativeOffsetFormat::writeAddressPlaceHolder(DataWriter& writer)
{
    writer.binary().append(m_offsetFormat->writePlaceHolder());
}
*/
} // namespace fuse::binary