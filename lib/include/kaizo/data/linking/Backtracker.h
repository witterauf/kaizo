#pragma once

#include "kaizo/data/linking/Constraint.h"
#include "kaizo/data/linking/FreeSpace.h"
#include "kaizo/data/linking/LinkObject.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <vector>

namespace kaizo::data {

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

class BacktrackingPacker
{
public:
    using ScoringStrategy = PriorityObjectList::ScoreObjectStrategy;

    BacktrackingPacker();

    void setLogFile(const std::filesystem::path& log);

    void setScoringStrategy(ScoringStrategy strategy);
    void setFreeBlocks(std::vector<FreeBlock>&& blocks);
    void addFreeBlock(const FreeBlock& block);
    void addObject(std::unique_ptr<LinkObject>&& object);
    auto object(const size_t index) const -> LinkObject*;
    auto objectCount() const -> size_t;

    bool pack();

private:
    FreeSpace m_freeSpace;
    PriorityObjectList m_objects;
    std::vector<std::unique_ptr<LinkObject>> m_linkObjects;
    size_t m_objectSize{0};

    struct BacktrackingState
    {
    public:
        BacktrackingState(std::vector<Allocation>&& allocations);

        bool hasMore() const;
        bool next();
        auto allocation() const -> Allocation;

    private:
        std::vector<Allocation> m_allocations;
        size_t m_allocationIndex{0};
        size_t m_allocationOffset{0};
        size_t m_shift{0};
        bool m_isAtEnd{true};
    };

    std::stack<BacktrackingState> m_state;

    void initializeLogging();
    void log_StartPacking();
    void log_InstantFail();
    void log_Allocated(const LinkObject&);
    void log_Deallocated(const LinkObject&);
    void log_Success();
    void log_Fail();

    std::optional<std::filesystem::path> m_logFile;
    std::ofstream m_log;
};

} // namespace kaizo::data