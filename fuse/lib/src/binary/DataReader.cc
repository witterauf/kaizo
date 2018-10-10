#include <diagnostics/Contracts.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/DataReader.h>

namespace fuse::binary {

DataReader::DataReader(const std::filesystem::path& filename)
{
    m_source = Binary::load(filename);
}

DataReader::~DataReader() = default;

auto DataReader::binary() const -> const Binary&
{
    return m_source;
}

auto DataReader::offset() const -> size_t
{
    return m_offset;
}

void DataReader::advance(size_t size)
{
    Expects(m_offset + size <= m_source.size());
    m_offset += size;
}

void DataReader::setOffset(size_t offset)
{
    Expects(offset <= m_source.size());
    m_offset = offset;
}

void DataReader::clearStorage()
{
    m_storage.clear();
}

void DataReader::store(const std::string& label, std::unique_ptr<Data>&& data)
{
    m_storage.insert(std::make_pair(label, std::move(data)));
}

bool DataReader::has(const std::string& label) const
{
    return m_storage.find(label) != m_storage.cend();
}

auto DataReader::load(const std::string& label) const -> const Data&
{
    Expects(has(label));
    return *m_storage.at(label);
}

} // namespace fuse::binary