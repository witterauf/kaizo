#pragma once

#include "Target.h"
#include <fuse/addresses/Address.h>
#include <map>
#include <vector>

namespace fuse {

class TargetMap
{
public:
    void addTarget(const Target& target);
    auto targetCount() const -> size_t;
    auto target(size_t index) const -> const Target&;
    bool hasTarget(Address address) const;
    auto target(Address address) const -> const Target&;

    auto begin() const
    {
        return m_targets.cbegin();
    }

    auto end() const
    {
        return m_targets.cend();
    }

private:
    std::vector<Target> m_targets;
};

} // namespace fuse