#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace fuse::text {

class TextSequence final
{
public:
    enum class Kind
    {
        Text,
        TableSwitch,
        Control,
        End
    };

    enum class ParameterFormat
    {
        Hexadecimal,
        Decimal,
        Binary
    };

    enum class Endianess
    {
        Little,
        Big
    };

    struct Parameter
    {
        std::string name;
        unsigned int size{1};
        Endianess endianess{Endianess::Little};
        ParameterFormat format{ParameterFormat::Hexadecimal};
    };

    struct Label
    {
        std::string name;
        size_t lineBreaks{0};
    };

    static auto makeText(const std::string& text) -> TextSequence;
    static auto makeTableSwitch(const std::string& table) -> TextSequence;
    static auto makeEnd(const Label& label) -> TextSequence;
    static auto makeControl(const Label& label, const std::vector<Parameter>& parameters)
        -> TextSequence;

    auto kind() const -> Kind;
    bool isText() const;
    bool isTableSwitch() const;
    bool isControl() const;
    bool isEnd() const;

    auto targetTable() const -> const std::string&;
    auto label() const -> const Label&;
    auto labelName() const -> const std::string&;
    auto text() const -> const std::string&;

    auto parameterCount() const -> size_t;
    auto parameter(size_t index) -> Parameter&;
    auto parameter(size_t index) const -> const Parameter&;

private:
    Kind m_kind;
    std::string m_string;
    Label m_label;
    std::vector<Parameter> m_parameters;
};

using BinarySequence = std::string;

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

        auto text() const -> const TextSequence&
        {
            return *m_text;
        }

    private:
        explicit EntryReference(const BinarySequence* binary, const TextSequence* text)
            : m_binary{binary}
            , m_text{text}
        {
        }

        const BinarySequence* m_binary;
        const TextSequence* m_text;
    };

    void setName(const std::string& name);
    auto name() const -> const std::string&;
    bool isAnonymous() const;

    auto size() const -> size_t;
    bool hasControl(const std::string& label) const;
    auto control(const std::string& label) const -> std::optional<EntryReference>;

    void insert(const BinarySequence& binary, const TextSequence& text);

    // matching algorithms
    template <class InputIterator>
    auto findLongestBinaryMatch(InputIterator begin, InputIterator end) const
        -> std::optional<EntryReference>;
    template <class InputIterator>
    auto findLongestTextMatch(InputIterator begin, InputIterator end) const
        -> std::optional<EntryReference>;

private:
    std::string m_name;
    std::map<BinarySequence, TextSequence> m_mapping;
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