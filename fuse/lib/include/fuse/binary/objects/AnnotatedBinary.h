#pragma once

#include "Object.h"
#include "UnresolvedReference.h"
#include <cstddef>
#include <filesystem>
#include <memory>
#include <vector>

namespace kaizo::data {

class AnnotatedBinary
{
    friend class AnnotatedBinaryDeserializer;

public:
    // in-flight object construction
    void startObject(const DataPath& path);
    void enter(const DataPathElement& child);
    void leave();
    auto binary() -> Binary&;
    auto binary() const -> const Binary&;
    void skip(size_t size);
    void addUnresolvedReference(const std::shared_ptr<AddressLayout>& format,
                                const DataPath& destination);
    void setFixedOffset(const size_t offset);
    void endObject();

    void append(AnnotatedBinary&& other);

    auto objectCount() const -> size_t;
    auto object(size_t index) const -> const Object*;
    auto object(size_t index) -> Object*;

private:
    auto relativeOffset() const -> size_t;

    DataPath m_currentPath;
    std::vector<std::unique_ptr<PackedObject>> m_objects;
    mutable std::optional<std::filesystem::path> m_binaryPath;

    Binary m_binary;
    std::unique_ptr<PackedObject> m_currentObject;
    size_t m_nextRealOffset;
};

} // namespace kaizo::data