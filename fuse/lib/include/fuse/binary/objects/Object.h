#pragma once

#include "UnresolvedReference.h"
#include <fuse/binary/DataPath.h>
#include <vector>

namespace fuse {

class LuaWriter;
class AnnotatedBinary;

class Object
{
public:
    struct Section
    {
        size_t offset{0};
        size_t realOffset{0};
        size_t size{0};
    };

    virtual auto path() const -> const binary::DataPath& = 0;
    virtual auto realSize() const -> size_t = 0;
    virtual auto size() const -> size_t = 0;
    virtual auto sectionCount() const -> size_t = 0;
    virtual auto section(size_t index) const -> const Section& = 0;
    virtual auto unresolvedReferenceCount() const -> size_t = 0;
    virtual auto unresolvedReference(size_t index) const -> const UnresolvedReference& = 0;
    virtual void serialize(LuaWriter& writer) const = 0;
};

class LuaReader;

class PackedObject : public Object
{
    friend class PackedObjectDeserializer;

public:
    static auto deserialize(LuaReader& reader, AnnotatedBinary* parent)
        -> std::unique_ptr<PackedObject>;

    PackedObject() = default;
    explicit PackedObject(const binary::DataPath& path, AnnotatedBinary* parent, size_t offset);

    void changeOffset(size_t offset);
    void addSection(size_t realOffset, size_t sectionSize);
    void addUnresolvedReference(const UnresolvedReference& reference);
    auto path() const -> const binary::DataPath& override;
    auto realSize() const -> size_t override;
    auto size() const -> size_t override;
    auto offset() const -> size_t;
    auto sectionCount() const -> size_t override;
    auto section(size_t index) const -> const Section& override;
    auto unresolvedReferenceCount() const -> size_t override;
    auto unresolvedReference(size_t index) const -> const UnresolvedReference& override;

    void serialize(LuaWriter& writer) const override;

private:
    void serializeAttributes(LuaWriter& writer) const;
    void serializeSections(LuaWriter& writer) const;
    void serializeReferences(LuaWriter& writer) const;

    AnnotatedBinary* m_parent{nullptr};
    binary::DataPath m_path;
    size_t m_offset{0};
    std::vector<Section> m_sections;
    std::vector<UnresolvedReference> m_references;
};

} // namespace fuse