#pragma once

#include "UnresolvedReference.h"
#include <fuse/binary/DataPath.h>
#include <optional>
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

    virtual ~Object() = default;

    void setAlignment(const size_t alignment);
    auto alignment() const -> size_t;
    void setFixedOffset(const size_t offset);
    bool hasFixedOffset() const;
    auto fixedOffset() const -> size_t;

    virtual auto binary() const -> fuse::Binary = 0;
    virtual auto path() const -> const binary::DataPath& = 0;
    virtual auto realSize() const -> size_t = 0;
    virtual auto size() const -> size_t = 0;
    virtual auto toPackedOffset(size_t offset) const -> size_t = 0;
    virtual auto toRealOffset(size_t offset) const -> size_t = 0;
    virtual auto sectionCount() const -> size_t = 0;
    virtual auto section(size_t index) const -> const Section& = 0;
    virtual auto sectionBinary(size_t index) const -> Binary = 0;
    virtual auto unresolvedReferenceCount() const -> size_t = 0;
    virtual auto unresolvedReference(size_t index) const -> const UnresolvedReference& = 0;
    virtual auto solveReference(size_t index, const Address address) const
        -> std::vector<BinaryPatch> = 0;
    virtual void serialize(LuaWriter& writer) const = 0;

private:
    size_t m_alignment{1};
    std::optional<size_t> m_fixedOffset;
};

class LuaDomReader;

class PackedObject : public Object
{
    friend class PackedObjectDeserializer;

public:
    static auto deserialize(LuaDomReader& reader, AnnotatedBinary* parent)
        -> std::unique_ptr<PackedObject>;

    PackedObject() = default;
    explicit PackedObject(const binary::DataPath& path, AnnotatedBinary* parent, size_t offset = 0);

    auto binary() const -> fuse::Binary override;
    void changeOffset(size_t offset);
    void addSection(size_t realOffset, size_t sectionSize);
    void addUnresolvedReference(const UnresolvedReference& reference);
    auto path() const -> const binary::DataPath& override;
    auto realSize() const -> size_t override;
    auto size() const -> size_t override;
    auto offset() const -> size_t;
    auto toPackedOffset(size_t offset) const -> size_t override;
    auto toRealOffset(size_t offset) const -> size_t override;
    auto sectionCount() const -> size_t override;
    auto section(size_t index) const -> const Section& override;
    auto sectionBinary(size_t index) const -> Binary override;
    auto unresolvedReferenceCount() const -> size_t override;
    auto unresolvedReference(size_t index) const -> const UnresolvedReference& override;
    auto solveReference(size_t index, const Address address) const
        -> std::vector<BinaryPatch> override;

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