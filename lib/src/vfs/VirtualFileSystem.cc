#include "kaizo/vfs/VirtualFileSystem.h"
#include <regex>
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

auto VirtualFileSystem::glob(const std::string& pattern) const -> std::vector<size_t>
{
    const std::regex regex{pattern};
    std::vector<size_t> matches;
    for (size_t index = 0; index < fileCount(); ++index)
    {
        if (std::regex_match(fileName(index), regex))
        {
            matches.push_back(index);
        }
    }
    return matches;
}

bool RegularFile::isFolder() const
{
    return false;
}

auto RegularFile::openAsFolder(Binary &&) -> std::unique_ptr<VirtualFileSystem>
{
    throw std::runtime_error{"file is not a folder"};
}

auto RegularFile::open(Binary&& binary) -> Binary
{
    return std::move(binary);
}

} // namespace kaizo