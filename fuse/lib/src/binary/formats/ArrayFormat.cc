#include <diagnostics/Contracts.h>
#include <fuse/binary/data/ArrayData.h>
#include <fuse/binary/formats/ArrayFormat.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/DataWriter.h>

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
        reader.enter(DataPathElement::makeIndex(i));
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
    if (data.type() != DataType::Array)
    {
        return throw std::runtime_error{"type mismatch"};
    }
    auto const& arrayData = static_cast<const ArrayData&>(data);

    for (auto i = 0U; i < arrayData.elementCount(); ++i)
    {
        writer.enter(DataPathElement::makeIndex(i));
        m_elementFormat->encode(writer, arrayData.element(i));
        writer.leave();
    }
}

auto ArrayFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto arrayFormat = std::make_unique<ArrayFormat>();
    arrayFormat->m_elementFormat = m_elementFormat->copy();
    arrayFormat->m_sizeProvider = m_sizeProvider->copy();
    copyDataFormat(*arrayFormat);
    return std::move(arrayFormat);
}

} // namespace fuse::binary