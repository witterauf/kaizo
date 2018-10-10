#include <fuse/binary/DataReader.h>
#include <fuse/binary/StringData.h>
#include <fuse/binary/StringFormat.h>

namespace fuse::binary {

void TableStringFormat::addTable(text::Table&& table)
{
    m_decoder.addTable(std::move(table));
}

auto TableStringFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    try
    {
        auto [newOffset, string] = m_decoder.decode(reader.binary(), reader.offset());
        auto data = std::make_unique<StringData>();
        data->setValue(string);
        reader.setOffset(newOffset);
        return std::move(data);
    }
    catch (...)
    {
        return {};
    }
}

auto TableStringFormat::copy() const -> std::unique_ptr<DataFormat>
{
    auto data = std::make_unique<TableStringFormat>();
    data->m_decoder = m_decoder;
    return std::move(data);
}

} // namespace fuse::binary