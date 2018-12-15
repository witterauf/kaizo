#include <diagnostics/Contracts.h>
#include <fuse/addresses/AbsoluteOffset.h>
#include <fuse/addresses/IdempotentAddressMap.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/data/Data.h>

namespace fuse::binary {

DataReader::DataReader(const std::filesystem::path& filename)
{
    m_source = Binary::load(filename);
    m_addressMap = std::make_unique<IdempotentAddressMap>(fileOffsetFormat());
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

void DataReader::enter(const DataPathElement& element)
{
    Expects(element.isStatic());
    m_path /= element;

    auto structure = std::make_unique<DataStructure>();
    structure->pathElement = element;
    structure->data = nullptr;
    structure->parent = m_currentNode;
    m_currentNode->children.push_back(std::move(structure));
    m_currentNode = m_currentNode->children.back().get();
}

void DataReader::trackRange(const std::string& tag, size_t address, size_t size)
{
    if (m_dataRangeConsumer)
    {
        m_dataRangeConsumer->track(m_path, DataRangeConsumer::Range{address, size}, tag);
    }
}

void DataReader::leave(const Data* data)
{
    m_currentNode->data = data;
    m_currentNode = m_currentNode->parent;
    m_path.goUp();
}

void DataReader::setAddressMap(std::unique_ptr<AddressMap>&& addressMap)
{
    Expects(addressMap);
    m_addressMap = std::move(addressMap);
}

auto DataReader::addressMap() const -> const AddressMap&
{
    return *m_addressMap;
}

} // namespace fuse::binary