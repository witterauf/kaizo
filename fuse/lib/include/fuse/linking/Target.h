#pragma once

#include "FreeBlock.h"
#include <filesystem>
#include <fuse/addresses/AddressMap.h>

namespace fuse {

class Target
{
public:
    explicit Target(const std::string& id, std::shared_ptr<AddressMap> addressMap = nullptr);

    auto id() const -> const std::string&;
    void setPath(const std::filesystem::path& path);
    auto path() const -> const std::filesystem::path&;
    void setOutputPath(const std::filesystem::path& path);
    auto outputPath() const -> const std::filesystem::path&;
    void mapFreeBlock(const FreeBlock& block);
    auto freeBlockCount() const -> size_t;
    auto freeBlock(size_t index) const -> const FreeBlock&;
    bool coversAddress(const Address address) const;
    auto toOffset(const Address address) const -> size_t;

private:
    std::string m_id;
    std::filesystem::path m_fileName;
    std::filesystem::path m_outputPath;
    std::shared_ptr<AddressMap> m_addressMap;
    std::vector<FreeBlock> m_freeBlocks;
};

} // namespace fuse