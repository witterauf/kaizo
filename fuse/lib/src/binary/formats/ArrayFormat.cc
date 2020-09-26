#include "FormatHelpers.h"
#include <diagnostics/Contracts.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>
#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/formats/ArrayFormat.h>

namespace fuse::binary {

void ArrayFormat::setSizeProvider(std::unique_ptr<ArraySizeProvider>&& sizeProvider)
{
    Expects(sizeProvider);
    m_sizeProvider = std::move(sizeProvider);
}

void ArrayFormat::setElementFormat(std::unique_ptr<DataFormat>&& format)
{
    Expects(format);
    m_elementFormat = std::move(format);
}

auto ArrayFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    Expects(m_sizeProvider);
    Expects(m_elementFormat);

    auto arrayData = std::make_unique<ArrayData>();
    auto const size = m_sizeProvider->provideSize(reader);
    for (auto i = 0U; i < size; ++i)
    {
        reader.enter(DataPathElement::makeIndex(i + 1));
        if (auto data = m_elementFormat->decode(reader))
        {
            reader.leave(data.get());
            arrayData->append(std::move(data));
        }
        else
        {
            reader.leave(data.get());
            return {};
        }
    }
    return std::move(arrayData);
}

void ArrayFormat::doEncode(DataWriter& writer, const Data& data)
{
    expectDataType(DataType::Array, data, writer.path());
    auto const& arrayData = static_cast<const ArrayData&>(data);

    for (auto i = 0U; i < arrayData.elementCount(); ++i)
    {
        writer.enter(DataPathElement::makeIndex(i + 1));
        m_elementFormat->encode(writer, arrayData.element(i));
        writer.leave();
    }
}

ArrayFormat::ArrayFormat(const ArrayFormat& other)
    : DataFormat{other}
    , m_elementFormat{other.m_elementFormat->copy()}
{
    if (other.m_sizeProvider)
    {
        m_sizeProvider = other.m_sizeProvider->copy();
    }
}

auto ArrayFormat::copy() const -> std::unique_ptr<DataFormat>
{
    return std::unique_ptr<ArrayFormat>{new ArrayFormat{*this}};
}

} // namespace fuse::binary