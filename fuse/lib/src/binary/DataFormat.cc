#include <fuse/binary/Data.h>
#include <fuse/binary/DataFormat.h>
#include <fuse/binary/DataReader.h>

namespace fuse::binary {

void DataFormat::storeAs(const std::string& label)
{
    m_storeAs = label;
}

void DataFormat::doNotStore()
{
    m_storeAs = {};
}

auto DataFormat::decode(DataReader& reader) -> std::unique_ptr<Data>
{
    auto data = doDecode(reader);
    if (m_storeAs)
    {
        reader.store(*m_storeAs, data->copy());
    }
    return std::move(data);
}

} // namespace fuse::binary