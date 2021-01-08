#include "Target.h"
#include <diagnostics/Contracts.h>

namespace fuse {

Target::Target(const std::string& id, std::shared_ptr<AddressMap> addressMap)
    : m_id{id}
    , m_addressMap{addressMap}
{
    Expects(!id.empty());
}

auto Target::id() const -> const std::string&
{
    return m_id;
}

void Target::setOutputPath(const std::filesystem::path& path)
{
    m_outputPath = path;
}

auto Target::outputPath() const -> const std::filesystem::path&
{
    return m_outputPath;
}

void Target::setPath(const std::filesystem::path& path)
{
    m_fileName = path;
}

auto Target::path() const -> const std::filesystem::path&
{
    return m_fileName;
}

void Target::mapFreeBlock(const FreeBlock& block)
{
    if (auto maybeAddress = m_addressMap->toTargetAddress(block.address()))
    {
        m_freeBlocks.push_back(FreeBlock{*maybeAddress, block.size()});
    }
    else
    {
        throw std::runtime_error{"could not map address " + block.address().toString() +
                                 " for target " + path().string()};
    }
}

auto Target::freeBlockCount() const -> size_t
{
    return m_freeBlocks.size();
}

auto Target::freeBlock(size_t index) const -> const FreeBlock&
{
    return m_freeBlocks[index];
}

bool Target::coversAddress(const Address address) const
{
    return m_addressMap->coversTargetAddress(address);
}

auto Target::toOffset(const Address address) const -> size_t
{
    Expects(coversAddress(address));
    return m_addressMap->toSourceAddresses(address).front().toInteger();
}

} // namespace fuse