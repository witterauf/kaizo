#include "kaizo/vfs/VirtualFileSystem.h"
#include <stdexcept>

namespace kaizo {

bool VirtualFileSystem::isFolder(const size_t index) const
{
    return fileInfo(index).isFolder;
}

bool VirtualFileSystem::isFile(const size_t index) const
{
    return !fileInfo(index).isFolder;
}

auto VirtualFileSystem::fileSize(const size_t index) const -> size_t
{
    return fileInfo(index).size;
}

bool RegularFile::isFolder() const
{
    return false;
}

auto RegularFile::openAsFolder(fuse::Binary &&) -> std::unique_ptr<VirtualFileSystem>
{
    throw std::runtime_error{"file is not a folder"};
}

auto RegularFile::open(fuse::Binary&& binary) -> fuse::Binary
{
    return std::move(binary);
}

} // namespace kaizo