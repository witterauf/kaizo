#pragma once

#include "AnnotatedBinary.h"
#include "DataPath.h"
#include "SegmentedBinary.h"
#include <fuse/Binary.h>
#include <fuse/addresses/Address.h>
#include <map>
#include <vector>

namespace fuse::binary {

class DataWriter
{
public:
    DataWriter();

    void startData(const DataPath& root);
    void finishData();
    auto assemble() const -> AnnotatedBinary;

    // DataFormat interface
    auto binary() -> Binary&;
    void enter(const DataPathElement& element);
    void enterLevel();
    void skip(size_t size);
    void leaveLevel();
    void leave();

    void addUnresolvedReference(const std::shared_ptr<ReferenceFormat>& format);

private:
    struct Section
    {
        AnnotatedBinary annotated;
        DataPath referencePath;
        size_t referenceOffset{0};
        std::vector<UnresolvedReference> unresolvedReferences;
    };

    auto section(int relative = 0) -> Section&;
    auto section(int relative = 0) const -> const Section&;

    std::vector<Section> m_sections;
    size_t m_sectionIndex{0};
    DataPath m_path;
    DataPath m_root;
    size_t m_lastPlacedOffset{0};
};

} // namespace fuse::binary