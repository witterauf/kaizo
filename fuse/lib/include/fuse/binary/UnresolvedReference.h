#pragma once

#include "DataPath.h"
#include <fuse/Binary.h>
#include <fuse/addresses/Address.h>
#include <fuse/addresses/AddressStorageFormat.h>
#include <memory>
#include <optional>

namespace fuse::binary {

class UnresolvedReference
{
public:
    UnresolvedReference() = default;
    explicit UnresolvedReference(const DataPath& path, size_t offset);

    bool isValid() const;
    void setDestination(const DataPath& path);
    void setFormat(const std::shared_ptr<AddressStorageFormat>& format);

private:
    DataPath m_sourcePath;
    size_t m_relativeOffset;
    DataPath m_destinationPath;
    std::shared_ptr<AddressStorageFormat> m_format;
};

} // namespace fuse::binary