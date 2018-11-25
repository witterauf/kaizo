#pragma once

#include "Address.h"
#include "AnnotatedBinary.h"
#include "DataPath.h"
#include <fuse/Binary.h>
#include <map>
#include <vector>

namespace fuse::binary {

class DataWriter
{
public:
    auto assemble() const -> AnnotatedBinary;

    auto binary() -> Binary&;
    void enter(const DataPathElement& element);
    void enterLevel();
    void skip(size_t size);
    void leaveLevel();
    void leave();

    void addUnresolvedReference(const std::shared_ptr<ReferenceFormat>& format);

private:
    struct OffsetBinary
    {
        size_t offset;
        Binary binary;
    };

    struct Section
    {
        DataPath referencePath;
        size_t referenceOffset;

        std::vector<OffsetBinary> binaries;
        std::map<DataPath, size_t> dataOffsets;
        std::vector<UnresolvedReference> unresolvedReferences;
    };

    auto section(int relative = 0) -> Section&;
    auto section(int relative = 0) const -> const Section&;
    auto sectionOffset(int relative = 0) const -> size_t;

    std::vector<Section> m_sections;
    size_t m_sectionIndex{0};
    DataPath m_path;
    size_t m_lastPlacedOffset{0};
};

} // namespace fuse::binary