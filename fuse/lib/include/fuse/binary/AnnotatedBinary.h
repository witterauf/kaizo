#pragma once

#include "UnresolvedReference.h"
#include <cstddef>
#include <map>
#include <vector>

namespace fuse {

class AnnotatedBinary
{
public:
    void startObject(const binary::DataPath& path);
    auto binary() -> Binary&;
    auto binary() const -> const Binary&;
    void skip(size_t size);
    void endObject();

    void append(const AnnotatedBinary& other);

    auto objectCount() const -> size_t;
    auto unresolvedReferenceCount() const -> size_t;

private:
    struct Section
    {
        size_t offset;
        size_t realOffset;
    };

    struct Object
    {
        Object() = default;
        explicit Object(size_t offset)
            : offset{offset}
        {
        }

        size_t offset{0};
        size_t size{0};
        std::vector<Section> sections{Section{0, 0}};
    };

    binary::DataPath m_currentPath;
    Object m_currentObject;

    std::map<binary::DataPath, Object> m_objects;
    std::vector<binary::UnresolvedReference> m_references;
    Binary m_binary;
};

} // namespace fuse