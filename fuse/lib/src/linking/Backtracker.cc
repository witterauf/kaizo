#include "fuse/linking/Backtracker.h"
#include <algorithm>
#include <contracts/Contracts.h>
#include <iostream>
#include <type_traits>

namespace kaizo::data {

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
    // TODO: sort again? expensive, but better results?
}

auto PriorityObjectList::unmapLast()
{
    m_unmapped.push_back(m_mapped.back());
    m_mapped.pop_back();
    // TODO: sort again?
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

void BacktrackingPacker::addObject(std::unique_ptr<LinkObject>&& object)
{
    if (object->hasFixedAddress())
    {
        throw std::runtime_error{"TODO: reimplement"};
        // No need to pack objects that have a fixed address anyway.
        // However, block other objects from using its address range.
        // m_freeSpace.allocateRange(object->allocation().address, object->size());
    }
    else
    {
        m_objects.addObject(object.get());
        m_objectSize += object->size();
        m_linkObjects.push_back(std::move(object));
    }
}

auto BacktrackingPacker::object(const size_t index) const -> LinkObject*
{
    return m_linkObjects[index].get();
}

auto BacktrackingPacker::objectCount() const -> size_t
{
    return m_linkObjects.size();
}

bool BacktrackingPacker::pack()
{
    initializeLogging();
    log_StartPacking();

    if (m_objectSize > m_freeSpace.capacity())
    {
        log_InstantFail();
        return false;
    }

    m_state.push(BacktrackingState{m_objects.nextUnmapped()->findAllocations(m_freeSpace)});
    while (!m_state.empty())
    {
        while (m_state.top().hasMore())
        {
            if (m_objects.canBeSatisfied())
            {
                auto const allocation = m_state.top().allocation();
                auto const size = m_objects.nextUnmapped()->size();
                // std::cout << "trying '" << m_objects.nextUnmapped()->path().toString() << " @ "
                //<< address.toString() << "\n";
                m_objects.nextUnmapped()->setAllocation(allocation);
                // TODO: use method that ensures the range exists
                m_freeSpace.allocateRange(allocation.address, size);
                log_Allocated(*m_objects.nextUnmapped());
                m_objects.mapNext();
                if (!m_objects.hasUnmapped())
                {
                    log_Success();
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
            log_Deallocated(*m_objects.nextUnmapped());
            m_objects.nextUnmapped()->unsetAllocation();
        }
    }

    log_Fail();
    return false;
}

void BacktrackingPacker::setLogFile(const std::filesystem::path& log)
{
    m_logFile = log;
}

BacktrackingPacker::BacktrackingState::BacktrackingState(std::vector<Allocation>&& allocations)
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

auto BacktrackingPacker::BacktrackingState::allocation() const -> Allocation
{
    Allocation shiftedAllocation;
    shiftedAllocation.size = m_allocations[m_allocationIndex].size;
    shiftedAllocation.block = m_allocations[m_allocationIndex].block;
    shiftedAllocation.offset = m_allocations[m_allocationIndex].offset + m_allocationOffset;
    shiftedAllocation.address =
        m_allocations[m_allocationIndex].address.applyOffset(m_allocationOffset);
    return shiftedAllocation;
}

bool BacktrackingPacker::BacktrackingState::hasMore() const
{
    return !m_isAtEnd;
}

void BacktrackingPacker::initializeLogging()
{
    if (m_logFile)
    {
        m_log.open(*m_logFile);
    }
}

void BacktrackingPacker::log_StartPacking()
{
    if (m_log.is_open())
    {
        m_log << "Start packing...\n";
        m_log << "Objects:\n";
        for (size_t i = 0; i < m_linkObjects.size(); ++i)
        {
            m_log << " [" << i << "] " << m_linkObjects[i]->id() << ": " << m_linkObjects[i]->size()
                  << "\n";
        }
    }
}

void BacktrackingPacker::log_InstantFail()
{
    if (m_log.is_open())
    {
        m_log << "Failed instantly due to not enough free space.\n";
    }
}

void BacktrackingPacker::log_Allocated(const LinkObject& object)
{
    if (m_log.is_open())
    {
        m_log << "  allocated object " << object.id() << " at offset " << object.allocation().offset
              << ", corresponding to address " << object.allocation().address.toString() << "\n";
    }
}

void BacktrackingPacker::log_Deallocated(const LinkObject& object)
{
    if (m_log.is_open())
    {
        m_log << "  deallocated object " << object.id() << "\n";
    }
}

void BacktrackingPacker::log_Success()
{
    if (m_log.is_open())
    {
        m_log << "Success!";
    }
}

void BacktrackingPacker::log_Fail()
{
    if (m_log.is_open())
    {
        m_log << "Fail.";
    }
}

} // namespace kaizo::data