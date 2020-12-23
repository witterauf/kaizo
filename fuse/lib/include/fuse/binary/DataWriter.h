#pragma once

#include "DataPath.h"
#include <fuse/Binary.h>
#include <fuse/addresses/Address.h>
#include <fuse/binary/objects/AnnotatedBinary.h>
#include <map>
#include <vector>

namespace fuse::binary {

class DataWriter
{
public:
    DataWriter();

    void startData(const DataPath& root);
    void finishData();
    auto assemble() -> AnnotatedBinary;

    auto binary() -> Binary&;

    // Manage object hierarchy
    void startNewObject();
    void enter(const DataPathElement& element);
    void reenter(const DataPathElement& element);
    auto path() const -> const DataPath&;
    auto entered() const -> DataPathElement;
    void enterLevel();
    void skip(size_t size);
    void leaveLevel();
    void leave();

    // Called by PointerFormat
    void addUnresolvedReference(const std::shared_ptr<AddressStorageFormat>& format,
                                const DataPath& destination);

private:
    struct Section
    {
        AnnotatedBinary annotated;
        DataPath referencePath;
        size_t referenceOffset{0};
    };

    void mergeSections();

    auto section(int relative = 0) -> Section&;
    auto section(int relative = 0) const -> const Section&;
    auto sectionOffset(int relative = 0) const -> size_t;

    std::vector<Section> m_sections;
    size_t m_sectionIndex{0};
    DataPath m_path;
    DataPath m_root;
    size_t m_lastPlacedOffset{0};
};

} // namespace fuse::binary