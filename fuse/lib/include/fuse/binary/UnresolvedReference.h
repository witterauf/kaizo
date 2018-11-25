#pragma once

#include "Address.h"
#include "DataPath.h"
#include <fuse/Binary.h>
#include <memory>
#include <optional>

namespace fuse::binary {

class ReferenceFormat
{
public:
    virtual auto asLua() const -> std::string = 0;
    virtual auto writeAddress(const Address address) const -> Binary = 0;
    virtual auto writePlaceHolder() const -> Binary = 0;
    virtual auto readAddress(const Binary& binary, size_t offset) const
        -> std::optional<std::pair<size_t, Address>> = 0;
};

class UnresolvedReference
{
public:
    UnresolvedReference() = default;
    explicit UnresolvedReference(const DataPath& path, size_t offset);

    bool isValid() const;
    void setDestination(const DataPath& path);
    void setFormat(const std::shared_ptr<ReferenceFormat>& format);

private:
    DataPath m_sourcePath;
    size_t m_relativeOffset;
    DataPath m_destinationPath;
    std::shared_ptr<ReferenceFormat> m_format;
};

} // namespace fuse::binary