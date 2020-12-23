#pragma once

#include "Constraint.h"
#include "FreeSpace.h"
#include "LinkObject.h"
#include "Packer.h"
#include <functional>

namespace fuse {

class PriorityObjectList
{
public:
    using ScoreObjectStrategy = std::function<long long(const FreeSpace&, const LinkObject&)>;

    explicit PriorityObjectList(const FreeSpace* freeSpace);

    PriorityObjectList();
    void setScoringStrategy(ScoreObjectStrategy strategy);
    void addObject(LinkObject* object);
    auto mapNext();
    auto unmapLast();
    auto nextUnmapped() const -> LinkObject*;
    bool canBeSatisfied() const;
    bool hasUnmapped() const;

private:
    void resortUnmappedObjects();

    const FreeSpace* m_freeSpace;

    struct ScoredObject
    {
        long long score;
        LinkObject* object;
    };
    std::vector<ScoredObject> m_unmapped;
    std::vector<ScoredObject> m_mapped;
    ScoreObjectStrategy m_scoreObject;
};

class BacktrackingPacker : public Packer
{
public:
    using ScoringStrategy = PriorityObjectList::ScoreObjectStrategy;

    BacktrackingPacker();

    void setScoringStrategy(ScoringStrategy strategy);
    void setFreeBlocks(std::vector<FreeBlock>&& blocks);
    void addFreeBlock(const FreeBlock& block) override;
    void addObject(LinkObject* object) override;

    bool pack() override;

private:
    FreeSpace m_freeSpace;
    PriorityObjectList m_objects;
    size_t m_objectSize{0};

    struct BacktrackingState
    {
    public:
        BacktrackingState(std::vector<AllocationCandidate>&& allocations);

        bool hasMore() const;
        bool next();
        auto allocation() const -> Address;

    private:
        std::vector<AllocationCandidate> m_allocations;
        size_t m_allocationIndex{0};
        size_t m_allocationOffset{0};
        size_t m_shift{0};
        bool m_isAtEnd{true};
    };

    std::stack<BacktrackingState> m_state;
};

} // namespace fuse