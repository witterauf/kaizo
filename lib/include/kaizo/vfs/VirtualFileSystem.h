#pragma once

#include <fuse/Binary.h>
#include <fuse/BinaryStream.h>
#include <memory>
#include <optional>
#include <string>

namespace kaizo {

class VirtualFileSystem
{
public:
    struct FileInfo
    {
        size_t size;
        bool isFolder;
    };

    virtual ~VirtualFileSystem() = default;
    virtual auto fileCount() const -> size_t = 0;
    virtual auto fileInfo(const size_t index) const -> FileInfo = 0;
    virtual auto fileName(const size_t index) const -> std::string = 0;
    virtual auto fileIndex(const std::string& name) const -> std::optional<size_t> = 0;
    virtual auto openFile(const size_t index) -> fuse::Binary = 0;
    virtual auto openFolder(const size_t index) -> std::unique_ptr<VirtualFileSystem> = 0;
    virtual auto glob(const std::string& pattern) const -> std::vector<size_t>;

    bool isFolder(const size_t index) const;
    bool isFile(const size_t index) const;
    auto fileSize(const size_t index) const -> size_t;
};

class FileTypeDescriptor
{
public:
    virtual ~FileTypeDescriptor() = default;
    virtual bool isFolder() const = 0;
    virtual auto openAsFolder(fuse::Binary&& binary) -> std::unique_ptr<VirtualFileSystem> = 0;
    virtual auto open(fuse::Binary&& binary) -> fuse::Binary = 0;
};

class RegularFile final : public FileTypeDescriptor
{
public:
    bool isFolder() const override;
    auto openAsFolder(fuse::Binary &&) -> std::unique_ptr<VirtualFileSystem> override;
    auto open(fuse::Binary&& binary) -> fuse::Binary override;
};

} // namespace kaizo