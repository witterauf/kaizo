#include "TargetMap.h"
#include <diagnostics/Contracts.h>
#include <fuse/FuseException.h>

namespace fuse {

void TargetMap::addTarget(const Target& target)
{
    m_targets.push_back(target);
}

auto TargetMap::targetCount() const -> size_t
{
    return m_targets.size();
}

auto TargetMap::target(size_t index) const -> const Target&
{
    Expects(index < targetCount());
    return m_targets[index];
}

bool TargetMap::hasTarget(Address address) const
{
    for (auto const& target : m_targets)
    {
        if (target.coversAddress(address))
        {
            return true;
        }
    }
    return false;
}

auto TargetMap::target(Address address) const -> const Target&
{
    for (auto const& target : m_targets)
    {
        if (target.coversAddress(address))
        {
            return target;
        }
    }
    throw FuseException{"no target covers address " + address.toString()};
}

} // namespace fuse