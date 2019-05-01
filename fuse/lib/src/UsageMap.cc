#include <algorithm>
#include <fuse/UsageMap.h>

namespace fuse {

void UsageMap::allocate(size_t start, size_t end)
{
    if (start < m_start)
    {
        m_start = start;
    }
    if (end > m_end)
    {
        m_end = end;
    }

    auto iter = m_usage.upper_bound(start);
    // If iter is begin(), then the first interval is already larger than start, so the argument
    // interval can not be in m_usage. Otherwise, iter is the first interval whose start is larger
    // than the argument interval's start, which means the previous interval could potentially
    // contain the argument interval, but only if its end is large than the argument end.
    if (iter != m_usage.begin() && (--iter)->second >= end)
    {
        return;
    }
    else
    {
        auto result = m_usage.insert(std::make_pair(start, end));
        iter = result.first;
    }
    mergeAt(iter);
}

void UsageMap::mergeAt(std::map<size_t, size_t>::iterator iter)
{
    // find start of consecutive intervals
    auto start = iter;
    auto startOffset = start->first;
    if (start != m_usage.begin())
    {
        if (std::prev(start)->second + 1 >= startOffset)
        {
            // previous interval overlaps with new interval
            --start;
        }
    }

    // find end of consecutive intervals
    auto end = iter;
    size_t endOffset;
    do
    {
        endOffset = end->second;
        ++end;
    } while (end != m_usage.end() && end->first <= iter->second);

    // reinsert as one interval
    if (start != end)
    {
        start->second = std::max(endOffset, iter->second);
        m_usage.erase(++start, end);
    }
}

void UsageMap::allocate(const std::pair<size_t, size_t>& interval)
{
    allocate(interval.first, interval.second);
}

static bool isBetween(size_t value, const std::pair<size_t, size_t>& interval)
{
    return interval.first <= value && value < interval.second;
}

bool UsageMap::isAllocated(size_t value) const
{
    auto iter = m_usage.upper_bound(value);
    return iter != m_usage.cbegin() && isBetween(value, *--iter);
}

auto UsageMap::startOffset() const -> size_t
{
    return m_start;
}

auto UsageMap::endOffset() const -> size_t
{
    return m_end;
}

auto UsageMap::begin() const -> const_iterator
{
    return m_usage.begin();
}

auto UsageMap::end() const -> const_iterator
{
    return m_usage.end();
}

auto UsageMap::cbegin() const -> const_iterator
{
    return m_usage.cbegin();
}

auto UsageMap::cend() const -> const_iterator
{
    return m_usage.cend();
}

auto UsageMap::size() const -> size_t
{
    return m_end - m_start;
}

} // namespace fuse
