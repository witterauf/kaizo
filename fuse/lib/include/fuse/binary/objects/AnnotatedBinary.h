#pragma once

#include "Object.h"
#include "UnresolvedReference.h"
#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

namespace fuse {

class LuaWriter;

class AnnotatedBinary
{
public:
    void startObject(const binary::DataPath& path);
    void enter(const binary::DataPathElement& child);
    void leave();
    auto binary() -> Binary&;
    auto binary() const -> const Binary&;
    void skip(size_t size);
    void addUnresolvedReference(const std::shared_ptr<AddressStorageFormat>& format,
                                const binary::DataPath& destination);
    void endObject();

    void serialize(LuaWriter& writer) const;
    void save(const std::filesystem::path& metaFile, const std::filesystem::path& binaryFile) const;
    void append(AnnotatedBinary&& other);

    auto objectCount() const -> size_t;

private:
    auto relativeOffset() const -> size_t;

    binary::DataPath m_currentPath;
    std::map<binary::DataPath, std::unique_ptr<PackedObject>> m_objects;
    mutable std::optional<std::filesystem::path> m_binaryPath;

    Binary m_binary;
    std::unique_ptr<PackedObject> m_currentObject;
    size_t m_nextRealOffset;
};

} // namespace fuse