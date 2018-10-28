#pragma once

#include "DataAnnotation.h"
#include <cstddef>
#include <fuse/Binary.h>
#include <map>

namespace fuse::binary {

class Data;

class DataReader
{
public:
    DataReader(const std::filesystem::path& filename);
    ~DataReader();

    auto binary() const -> const Binary&;
    auto offset() const -> size_t;
    void advance(size_t size);
    void setOffset(size_t offset);

    void enter(const DataPathElement& element);
    void annotateRange(size_t, size_t);
    void leave(const Data* data);

    struct Range
    {
        size_t address;
        size_t size;
    };

    auto ranges() const -> const DataAnnotation<Range>&;

private:
    size_t m_offset{0};
    Binary m_source;
    DataPath m_path;
    DataAnnotation<Range> m_ranges;

    struct DataStructure
    {
        DataPathElement pathElement;
        const Data* data;
        DataStructure* parent;
        std::vector<std::unique_ptr<DataStructure>> children;
    };

    DataStructure m_root;
    DataStructure* m_currentNode{&m_root};
};

} // namespace fuse::binary