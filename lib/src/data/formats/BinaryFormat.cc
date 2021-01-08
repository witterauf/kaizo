#include <contracts/Contracts.h>
#include <kaizo/data/data/BinaryData.h>
#include <kaizo/data/formats/BinaryFormat.h>
#include <kaizo/data/DataReader.h>

namespace kaizo::data {

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

} // namespace kaizo::data