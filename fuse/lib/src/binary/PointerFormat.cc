#include <diagnostics/Contracts.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/PointerFormat.h>
#include <fuse/binary/Data.h>

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

auto AbsolutePointerFormat::readAddress(DataReader& reader) -> std::optional<Address>
{
    if (auto maybeResult = addressFormat().read(reader.binary(), reader.offset()))
    {
        auto[newOffset, address] = *maybeResult;
        reader.setOffset(newOffset);
        return address;
    }
    return {};
}

} // namespace fuse::binary