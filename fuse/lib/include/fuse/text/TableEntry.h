#pragma once

#include <string>
#include <vector>

namespace fuse::text {

using BinarySequence = std::string;

class TableEntry final
{
public:
    enum class Kind
    {
        Text,
        TableSwitch,
        Control,
        End
    };

    struct ParameterFormat
    {
    public:
        enum class Display
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

        using argument_t = long;

        bool isCompatible(argument_t value) const;
        auto encode(argument_t value) const -> BinarySequence;
        template <class InputIterator> auto decode(InputIterator begin) -> argument_t;

        unsigned int size{1};
        Endianess endianess{Endianess::Little};
        Display preferedDisplay{Display::Hexadecimal};
    };

    struct Label
    {
        std::string name;
        std::string postfix;
    };

    static auto makeText(const std::string& text) -> TableEntry;
    static auto makeTableSwitch(const std::string& table) -> TableEntry;
    static auto makeEnd(const Label& label) -> TableEntry;
    static auto makeControl(const Label& label, const std::vector<ParameterFormat>& parameters)
        -> TableEntry;

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
    auto parameter(size_t index) -> ParameterFormat&;
    auto parameter(size_t index) const -> const ParameterFormat&;

private:
    Kind m_kind;
    std::string m_string;
    Label m_label;
    std::vector<ParameterFormat> m_parameters;
};

//##[ implementation ]#############################################################################

template <class InputIterator>
auto TableEntry::ParameterFormat::decode(InputIterator begin) -> argument_t
{
    if (endianess == Endianess::Little)
    {
        uint64_t value{0};
        for (auto i = 0U; i < size; ++i, ++begin)
        {
            value |= static_cast<uint8_t>(*begin) << (i * 8);
        }
        return static_cast<argument_t>(value);
    }
    else
    {
        uint64_t value{0};
        for (auto i = 0U; i < size; ++i, ++begin)
        {
            value <<= 8;
            value |= static_cast<uint8_t>(*begin);
        }
        return static_cast<argument_t>(value);
    }
}

} // namespace fuse::text