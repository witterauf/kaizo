#pragma once

#include "UnresolvedReference.h"
#include <fuse/binary/DataPath.h>
#include <vector>

namespace fuse {

class LuaWriter;

class Object
{
public:
    struct Section
    {
        size_t offset{0};
        size_t realOffset{0};
        size_t size{0};
    };

    Object() = default;
    explicit Object(const binary::DataPath& path, size_t offset);

    void changeOffset(size_t offset);
    void addSection(size_t realOffset, size_t sectionSize);
    void addUnresolvedReference(const UnresolvedReference& reference);
    auto path() const -> const binary::DataPath&;
    auto realSize() const -> size_t;
    auto size() const -> size_t;
    auto offset() const -> size_t;
    auto sectionCount() const -> size_t;
    auto section(size_t index) const -> const Section&;
    auto unresolvedReferenceCount() const -> size_t;
    auto unresolvedReference(size_t index) const -> const UnresolvedReference&;

    void serialize(LuaWriter& writer) const;

private:
    void serializeAttributes(LuaWriter& writer) const;
    void serializeSections(LuaWriter& writer) const;
    void serializeReferences(LuaWriter& writer) const;

    binary::DataPath m_path;
    size_t m_offset{0};
    std::vector<Section> m_sections;
    std::vector<UnresolvedReference> m_references;
};

} // namespace fuse