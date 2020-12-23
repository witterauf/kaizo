#include "Backtracker.h"
#include <algorithm>
#include <diagnostics/Contracts.h>
#include <iostream>
#include <type_traits>

namespace fuse {

static auto defaultScoringStrategy(const FreeSpace& space, const LinkObject& object) -> long long
{
    auto const slack = object.measureSlack(space);
    return -static_cast<long long>(
        std::min(slack, static_cast<size_t>(std::numeric_limits<long long>::max())));
}

PriorityObjectList::PriorityObjectList()
    : m_scoreObject{&defaultScoringStrategy}
{
}

PriorityObjectList::PriorityObjectList(const FreeSpace* freeSpace)
    : m_freeSpace{freeSpace}
    , m_scoreObject{&defaultScoringStrategy}
{
}

void PriorityObjectList::addObject(LinkObject* object)
{
    Expects(m_freeSpace);
    Expects(object);
    auto const score = m_scoreObject(*m_freeSpace, *object);
    m_unmapped.insert(
        std::upper_bound(m_unmapped.begin(), m_unmapped.end(), score,
                         [](auto const& score, auto const& b) { return score < b.score; }),
        ScoredObject{score, object});
}

void PriorityObjectList::setScoringStrategy(ScoreObjectStrategy strategy)
{
    m_scoreObject = strategy;
}

auto PriorityObjectList::mapNext()
{
    m_mapped.push_back(m_unmapped.back());
    m_unmapped.pop_back();
    // TODO: resort?
}

auto PriorityObjectList::unmapLast()
{
    m_unmapped.push_back(m_mapped.back());
    m_mapped.pop_back();
    // TODO: resort?
}

auto PriorityObjectList::nextUnmapped() const -> LinkObject*
{
    return m_unmapped.back().object;
}

bool PriorityObjectList::canBeSatisfied() const
{
    for (auto i = 0U; i < m_unmapped.size(); ++i)
    {
        auto const* object = m_unmapped[m_unmapped.size() - 1 - i].object;
        if (!object->hasAllocations(*m_freeSpace))
        {
            return false;
        }
    }
    return true;
}

void PriorityObjectList::resortUnmappedObjects()
{
    std::sort(m_unmapped.begin(), m_unmapped.end(),
              [](auto const& a, auto const& b) { return a.score < b.score; });
}

bool PriorityObjectList::hasUnmapped() const
{
    return !m_unmapped.empty();
}

BacktrackingPacker::BacktrackingPacker()
    : m_objects{&m_freeSpace}
{
}

void BacktrackingPacker::setScoringStrategy(ScoringStrategy strategy)
{
    m_objects.setScoringStrategy(strategy);
}

void BacktrackingPacker::setFreeBlocks(std::vector<FreeBlock>&& blocks)
{
    m_freeSpace = FreeSpace{std::move(blocks)};
}

void BacktrackingPacker::addFreeBlock(const FreeBlock& block)
{
    m_freeSpace.addBlock(block);
}

void BacktrackingPacker::addObject(LinkObject* object)
{
    if (object->hasFixedAddress())
    {
        // No need to pack objects that have a fixed address anyway.
        // However, block other objects from using its address range.
        m_freeSpace.allocateRange(object->address(), object->size());
    }
    else
    {
        m_objects.addObject(object);
        m_objectSize += object->size();
    }
}

bool BacktrackingPacker::pack()
{
    if (m_objectSize > m_freeSpace.capacity())
    {
        std::cout << m_objectSize << "\n";
        return false;
    }

    m_state.push(BacktrackingState{m_objects.nextUnmapped()->findAllocations(m_freeSpace)});
    while (!m_state.empty())
    {
        while (m_state.top().hasMore())
        {
            if (m_objects.canBeSatisfied())
            {
                auto const address = m_state.top().allocation();
                auto const size = m_objects.nextUnmapped()->size();
                //std::cout << "trying '" << m_objects.nextUnmapped()->path().toString() << " @ "
                          //<< address.toString() << "\n";
                m_objects.nextUnmapped()->setAddress(address);
                // TODO: use method that ensures the range exists
                m_freeSpace.allocateRange(address, size);
                m_objects.mapNext();
                if (!m_objects.hasUnmapped())
                {
                    return true;
                }
                m_state.push(
                    BacktrackingState{m_objects.nextUnmapped()->findAllocations(m_freeSpace)});
            }
            else
            {
                // Ran into a deadend. Try next allocation candidate.
                m_state.top().next();
            }
        }

        m_state.pop();
        if (!m_state.empty())
        {
            std::cout << "Backtracking...\n";
            m_freeSpace.deallocateLast();
            m_objects.unmapLast();
            m_objects.nextUnmapped()->unsetAddress();
        }
    }
    return false;
}

BacktrackingPacker::BacktrackingState::BacktrackingState(
    std::vector<AllocationCandidate>&& allocations)
    : m_allocations{std::move(allocations)}
{
    m_isAtEnd = m_allocations.empty();
}

bool BacktrackingPacker::BacktrackingState::next()
{
    if (m_allocations.empty() || m_isAtEnd)
    {
        return false;
    }

    // The space of possible allocations is huge, so this implements a kind of binary traversing.
    // Overall, this would take just as long as just traversing an allocation linearly, but we cover
    // a larger range in fewer iterations, making it possible to cancel earlier and saying there is
    // no viable allocation.
    m_allocationOffset += (m_allocations[m_allocationIndex].size >> m_shift);
    if (m_allocationOffset >= m_allocations[m_allocationIndex].size)
    {
        m_allocationOffset = 0;
        if (++m_allocationIndex == m_allocations.size())
        {
            m_allocationIndex = 0;
            m_shift += 1;
        }
        while (m_allocations[m_allocationIndex].size < (1ULL << m_shift))
        {
            if (++m_allocationIndex == m_allocations.size())
            {
                m_isAtEnd = true;
                return false;
            }
        }
    }
    return true;
}

auto BacktrackingPacker::BacktrackingState::allocation() const -> Address
{
    return m_allocations[m_allocationIndex].start.applyOffset(m_allocationOffset);
}

bool BacktrackingPacker::BacktrackingState::hasMore() const
{
    return !m_isAtEnd;
}

} // namespace fuse