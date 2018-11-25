#include <diagnostics/Contracts.h>
#include <fuse/binary/data/BinaryData.h>
#include <fuse/binary/formats/BinaryFormat.h>
#include <fuse/binary/DataReader.h>

namespace fuse::binary {

void BinaryFormat::setSizeProvider(std::unique_ptr<BinarySizeProvider>&& sizeProvider)
{
    m_sizeProvider = std::move(sizeProvider);
}

auto BinaryFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    Expects(m_sizeProvider);

    auto const size = m_sizeProvider->provideSize(reader);
    if (reader.offset() + size < reader.binary().size())
    {
        auto data = std::make_unique<BinaryData>();
        data->setData(reader.binary().read(reader.offset(), size));
        reader.advance(size);
        return std::move(data);
    }
    else
    {
        return {};
    }
}

} // namespace fuse::binary