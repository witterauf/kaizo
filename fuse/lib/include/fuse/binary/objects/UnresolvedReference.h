#pragma once

#include <fuse/Binary.h>
#include <fuse/addresses/Address.h>
#include <fuse/addresses/AddressStorageFormat.h>
#include <fuse/binary/DataPath.h>
#include <memory>
#include <optional>

namespace fuse {

class UnresolvedReference
{
public:
    UnresolvedReference() = default;
    explicit UnresolvedReference(const binary::DataPath& path, size_t offset);

    bool isValid() const;
    auto originPath() const -> const binary::DataPath&;
    auto referencedPath() const -> const binary::DataPath&;
    auto relativeOffset() const -> size_t;
    auto addressLayout() const -> const AddressStorageFormat&;

    void setDestination(const binary::DataPath& path);
    void setFormat(const std::shared_ptr<AddressStorageFormat>& format);

private:
    binary::DataPath m_sourcePath;
    size_t m_relativeOffset;
    binary::DataPath m_destinationPath;
    std::shared_ptr<AddressStorageFormat> m_format;
};

} // namespace fuse