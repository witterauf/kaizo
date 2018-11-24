#pragma once

#include "DataPath.h"
#include <fuse/Binary.h>
#include <map>
#include <vector>

namespace fuse::binary {

class DataWriter
{
public:
    auto binary() -> Binary&;

    void enter(const DataPathElement& element);
    void startSection();
    void skip(size_t size);
    void endSection();
    void leave();

private:
    std::vector<Binary> m_partialSections;
    std::map<DataPath, Binary> m_sections;
    DataPath m_path;
};

} // namespace fuse::binary