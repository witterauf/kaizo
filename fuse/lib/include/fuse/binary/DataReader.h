#pragma once

#include "DataAnnotation.h"
#include "DataRangeTracker.h"
#include <cstddef>
#include <fuse/Binary.h>
#include <fuse/BinaryView.h>
#include <fuse/addresses/AddressMap.h>
#include <map>
#include <memory>

namespace fuse::binary {

class Data;

class DataReader
{
public:
    DataReader(const BinaryView& binary);
    DataReader(const std::filesystem::path& filename);
    ~DataReader();

    auto binary() const -> const Binary&;
    auto dataSize() const -> size_t;
    auto offset() const -> size_t;
    void advance(size_t size);
    void setOffset(size_t offset);
    void setAddressMap(std::unique_ptr<AddressMap>&& addressMap);
    auto addressMap() const -> const AddressMap&;

    void enter(const DataPathElement& element);
    void setTracker(DataRangeTracker* tracker);
    void trackRange(const std::string& tag, size_t offset, size_t size);
    void leave(const Data* data);

private:
    size_t m_offset{0};
    std::unique_ptr<AddressMap> m_addressMap;
    Binary m_source;
    DataPath m_path;

    struct DataStructure
    {
        DataPathElement pathElement;
        const Data* data;
        DataStructure* parent;
        std::vector<std::unique_ptr<DataStructure>> children;
    };

    DataStructure m_root;
    DataStructure* m_currentNode{&m_root};
    DataRangeTracker* m_dataRangeConsumer{nullptr};
};

} // namespace fuse::binary