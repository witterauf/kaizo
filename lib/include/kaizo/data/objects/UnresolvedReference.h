#pragma once

#include <kaizo/binary/Binary.h>
#include <kaizo/addresses/Address.h>
#include <kaizo/addresses/AddressLayout.h>
#include <kaizo/data/DataPath.h>
#include <memory>
#include <optional>

namespace kaizo::data {

class UnresolvedReference
{
public:
    UnresolvedReference() = default;
    explicit UnresolvedReference(size_t offset);
    explicit UnresolvedReference(const DataPath& path, size_t offset);

    bool isValid() const;
    auto originPath() const -> const DataPath&;
    auto referencedPath() const -> const DataPath&;
    auto relativeOffset() const -> size_t;
    auto addressLayout() const -> const AddressLayout&;

    void setDestination(const DataPath& path);
    void setFormat(const std::shared_ptr<AddressLayout>& format);

private:
    DataPath m_sourcePath;
    size_t m_relativeOffset;
    DataPath m_destinationPath;
    std::shared_ptr<AddressLayout> m_format;
};

} // namespace kaizo::data