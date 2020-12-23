#include "LinkObject.h"
#include "FreeSpace.h"
#include <diagnostics/Contracts.h>
#include <fuse/binary/objects/Object.h>

namespace fuse {

LinkObject::LinkObject(const Object* object)
    : m_object{object}
{
    Expects(m_object);
    Expects(m_object->size() > 0);
    Expects(!m_object->path().isEmpty());
}

auto LinkObject::size() const -> size_t
{
    Expects(m_object);
    return m_object->size();
}

auto LinkObject::path() const -> const binary::DataPath&
{
    Expects(m_object);
    return m_object->path();
}

auto LinkObject::object() const -> const Object&
{
    Expects(m_object);
    return *m_object;
}

void LinkObject::setAddress(const Address address)
{
    Expects(!m_address);
    m_address = address;
}

void LinkObject::unsetAddress()
{
    m_address = {};
}

bool LinkObject::hasAddress() const
{
    return m_address.has_value();
}

void LinkObject::setFixedAddress(const Address address)
{
    m_address = address;
    m_hasFixedAddress = true;
}

bool LinkObject::hasFixedAddress() const
{
    return m_hasFixedAddress;
}

auto LinkObject::address() const -> const Address&
{
    Expects(hasAddress());
    return *m_address;
}

auto LinkObject::findAllocations(const FreeSpace& space) const -> std::vector<AllocationCandidate>
{
    if (m_constraint)
    {
        return m_constraint->findAllocations(space, size());
    }
    else
    {
        auto const blocks = space.findBlocksThatFit(size());
        std::vector<AllocationCandidate> allocations(blocks.size());
        for (auto i = 0U; i < blocks.size(); ++i)
        {
            allocations[i].start = space.block(blocks[i]).address();
            allocations[i].size = space.block(blocks[i]).size() - size();
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
        slack += allocation.size;
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

} // namespace fuse