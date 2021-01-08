#pragma once

#include <map>

namespace kaizo {

class UsageMap
{
public:
    using const_iterator = std::map<size_t, size_t>::const_iterator;

    UsageMap() = default;

    explicit UsageMap(size_t end)
        : m_start{0}
        , m_end{end}
    {
    }
    explicit UsageMap(size_t start, size_t end)
        : m_start{start}
        , m_end{end}
    {
    }

    void allocate(size_t start, size_t end);
    void allocate(const std::pair<size_t, size_t>& interval);
    bool isAllocated(size_t value) const;

    auto startOffset() const -> size_t;
    auto endOffset() const -> size_t;

    auto begin() const -> const_iterator;
    auto end() const -> const_iterator;
    auto cbegin() const -> const_iterator;
    auto cend() const -> const_iterator;

    auto size() const -> size_t;

private:
    void mergeAt(std::map<size_t, size_t>::iterator iter);

    size_t m_start{0}, m_end{0};
    std::map<size_t, size_t> m_usage;
};

} // namespace kaizo
