#include <diagnostics/Contracts.h>
#include <fuse/binary/Data.h>
#include <fuse/binary/DataReader.h>
#include <fuse/binary/address_formats/AbsoluteOffset.h>

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

void DataReader::annotateRange(size_t address, size_t size)
{
    m_ranges.annotate(m_path, Range{address, size});
}

void DataReader::leave(const Data* data)
{
    m_currentNode->data = data;
    m_currentNode = m_currentNode->parent;
    m_path.goUp();
}

auto DataReader::ranges() const -> const DataAnnotation<Range>&
{
    return m_ranges;
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