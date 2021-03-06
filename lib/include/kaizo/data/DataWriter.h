#pragma once

#include "DataPath.h"
#include <kaizo/addresses/Address.h>
#include <kaizo/binary/Binary.h>
#include <kaizo/data/objects/AnnotatedBinary.h>
#include <map>
#include <vector>

namespace kaizo::data {

class DataWriter
{
public:
    DataWriter();

    void startData(const DataPath& root);
    void finishData();
    void abortData();
    auto assemble() -> AnnotatedBinary;

    auto binary() -> Binary&;

    // Manage object hierarchy
    void startNewObject();
    void startNewObject(const size_t fixedOffset);
    void enter(const DataPathElement& element);
    void reenter(const DataPathElement& element);
    auto path() const -> const DataPath&;
    auto entered() const -> DataPathElement;
    void enterLevel();
    void skip(size_t size);
    void leaveLevel();
    void leave();

    // Called by PointerFormat
    void addUnresolvedReference(const std::shared_ptr<AddressLayout>& format,
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

} // namespace kaizo::data