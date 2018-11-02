#pragma once

#include "TableEntry.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace fuse::text {

class Table final
{
public:
    class EntryReference
    {
        friend class Table;

    public:
        auto binary() const -> const BinarySequence&
        {
            return *m_binary;
        }

        auto text() const -> const TableEntry&
        {
            return *m_text;
        }

    private:
        explicit EntryReference(const BinarySequence* binary, const TableEntry* text)
            : m_binary{binary}
            , m_text{text}
        {
        }

        const BinarySequence* m_binary;
        const TableEntry* m_text;
    };

    void setName(const std::string& name);
    auto name() const -> const std::string&;
    bool isAnonymous() const;

    auto size() const -> size_t;
    bool hasControl(const std::string& label) const;
    auto control(const std::string& label) const -> std::optional<EntryReference>;
    auto entry(size_t index) const -> EntryReference;

    void insert(const BinarySequence& binary, const TableEntry& text);

    // matching algorithms
    template <class InputIterator>
    auto findLongestBinaryMatch(InputIterator begin, InputIterator end) const
        -> std::optional<EntryReference>;
    template <class InputIterator>
    auto findLongestTextMatch(InputIterator begin, InputIterator end) const
        -> std::optional<EntryReference>;

private:
    std::string m_name;
    std::map<BinarySequence, TableEntry> m_mapping;
};

//##[ implementation ]#############################################################################

template <typename InputIterator>
auto Table::findLongestBinaryMatch(InputIterator begin, InputIterator end) const
    -> std::optional<EntryReference>
{
    BinarySequence sequence;
    auto match = m_mapping.cend();

    size_t length{1};
    while (begin != end)
    {
        sequence.push_back(*begin++);
        auto const pos = m_mapping.lower_bound(sequence);
        if (pos != m_mapping.cend())
        {
            if (pos->first == sequence)
            {
                match = pos;
            }
            else if (pos->first.substr(0, length) == sequence)
            {
                begin += length - 1;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
        ++length;
    }

    if (match != m_mapping.cend())
    {
        return EntryReference{&match->first, &match->second};
    }
    else
    {
        return {};
    }
}

} // namespace fuse::text