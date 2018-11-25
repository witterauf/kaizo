#pragma once

#include "UnresolvedReference.h"
#include <cstddef>
#include <map>
#include <vector>

namespace fuse::binary {

class AnnotatedBinary
{
public:
    auto objectCount() const -> size_t;
    auto unresolvedReferenceCount() const -> size_t;

private:
    struct Section
    {
        size_t fileOffset;
        size_t relativeOffset;
    };

    struct Object
    {
        std::vector<Section> sections;
    };

    std::map<DataPath, Object> m_dataOffsets;
    std::vector<UnresolvedReference> m_references;
    Binary m_binary;
};

} // namespace fuse::binary