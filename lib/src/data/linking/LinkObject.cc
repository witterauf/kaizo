#include "fuse/linking/LinkObject.h"
#include "fuse/linking/FreeSpace.h"
#include <contracts/Contracts.h>

namespace kaizo::data {

LinkObject::LinkObject(const std::string& id, const size_t size)
    : m_id{id}
    , m_size{size}
{
    Expects(!id.empty());
}

auto LinkObject::id() const -> const std::string&
{
    return m_id;
}

auto LinkObject::size() const -> size_t
{
    return m_size;
}

void LinkObject::setAllocation(const Allocation& allocation)
{
    Expects(!m_allocation);
    m_allocation = allocation;
}

void LinkObject::unsetAllocation()
{
    m_allocation.reset();
}

bool LinkObject::hasAllocation() const
{
    return m_allocation.has_value();
}

void LinkObject::setFixedAddress(const Address)
{
    // m_address = address;
    m_hasFixedAddress = true;
}

bool LinkObject::hasFixedAddress() const
{
    return m_hasFixedAddress;
}

auto LinkObject::allocation() const -> const Allocation&
{
    Expects(hasAllocation());
    return *m_allocation;
}

auto LinkObject::findAllocations(const FreeSpace& space) const -> std::vector<Allocation>
{
    if (m_constraint)
    {
        return m_constraint->findAllocations(space, size());
    }
    else
    {
        auto const blocks = space.findBlocksThatFit(size());
        std::vector<Allocation> allocations(blocks.size());
        for (auto i = 0U; i < blocks.size(); ++i)
        {
            allocations[i].address = space.block(blocks[i]).address();
            allocations[i].size = space.block(blocks[i]).size() - size();
            allocations[i].offset = space.block(blocks[i]).offset();
            allocations[i].block = blocks[i];
        }
        return std::move(allocations);
    }
}

bool LinkObject::hasAllocations(const FreeSpace& space) const
{
    if (m_constraint)
    {
        return m_constraint->hasAllocations(space, size());
    }
    else
    {
        return space.hasBlockThatFits(size());
    }
}

auto LinkObject::measureSlack(const FreeSpace& space) const -> size_t
{
    size_t slack{0};
    auto const allocations = findAllocations(space);
    for (auto const& allocation : allocations)
    {
        slack += allocation.size - size();
    }
    return slack;
}

void LinkObject::constrain(std::unique_ptr<Constraint>&& constraint)
{
    m_constraint = std::move(constraint);
}

bool LinkObject::isConstrained() const
{
    return m_constraint != nullptr;
}

bool LinkObject::isUnconstrained() const
{
    return m_constraint == nullptr;
}

} // namespace kaizo::data