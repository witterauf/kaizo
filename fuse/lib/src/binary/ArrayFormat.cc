#include <diagnostics/Contracts.h>
#include <fuse/binary/ArrayData.h>
#include <fuse/binary/ArrayFormat.h>
#include <fuse/binary/DataReader.h>

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
            arrayData->append(std::move(data));
            reader.leave(data.get());
        }
        else
        {
            reader.leave(data.get());
            return {};
        }
    }
    return std::move(arrayData);
}

} // namespace fuse::binary