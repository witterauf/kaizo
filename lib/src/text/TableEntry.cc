#include "kaizo/text/TableEntry.h"
#include <contracts/Contracts.h>

namespace kaizo::text {

bool TableEntry::ParameterFormat::isCompatible(argument_t value) const
{
    if (value < 0)
    {
        return false;
    }
    if (value >= (1LL << (size * 8)))
    {
        return false;
    }
    return true;
}

auto TableEntry::ParameterFormat::encode(argument_t value) const -> BinarySequence
{
    BinarySequence binary;
    if (endianess == Endianess::Little)
    {
        for (auto i = 0U; i < size; ++i)
        {
            binary += static_cast<BinarySequence::value_type>(value & 0xff);
            value >>= 8;
        }
    }
    else
    {
        for (auto i = size; i > 0; ++i)
        {
            binary += static_cast<BinarySequence::value_type>((value >> ((i - 1) * 8)) & 0xff);
        }
    }
    return binary;
}

auto TableEntry::makeText(const std::string& text) -> TableEntry
{
    Expects(!text.empty());
    TableEntry sequence;
    sequence.m_kind = Kind::Text;
    sequence.m_string = text;
    return sequence;
}

auto TableEntry::makeTableSwitch(const std::string& table) -> TableEntry
{
    Expects(!table.empty());
    TableEntry sequence;
    sequence.m_kind = Kind::TableSwitch;
    sequence.m_string = table;
    return sequence;
}

auto TableEntry::makeEnd(const Label& label) -> TableEntry
{
    Expects(!label.name.empty());
    TableEntry sequence;
    sequence.m_kind = Kind::End;
    sequence.m_label = label;
    return sequence;
}

auto TableEntry::makeControl(const Label& label, const std::vector<ParameterFormat>& parameters)
    -> TableEntry
{
    Expects(!label.name.empty());
    TableEntry sequence;
    sequence.m_kind = Kind::Control;
    sequence.m_label = label;
    sequence.m_parameters = parameters;
    return sequence;
}

auto TableEntry::kind() const -> Kind
{
    return m_kind;
}

auto TableEntry::targetTable() const -> const std::string&
{
    Expects(kind() == Kind::TableSwitch);
    return m_string;
}

auto TableEntry::label() const -> const Label&
{
    Expects(kind() == Kind::Control || kind() == Kind::End);
    return m_label;
}

auto TableEntry::labelName() const -> const std::string&
{
    Expects(kind() == Kind::Control || kind() == Kind::End);
    return m_label.name;
}

auto TableEntry::text() const -> const std::string&
{
    Expects(kind() == Kind::Text);
    return m_string;
}

auto TableEntry::parameterCount() const -> size_t
{
    return m_parameters.size();
}

auto TableEntry::parameter(size_t index) -> ParameterFormat&
{
    Expects(index < parameterCount());
    return m_parameters[index];
}

auto TableEntry::parameter(size_t index) const -> const ParameterFormat&
{
    Expects(index < parameterCount());
    return m_parameters[index];
}

bool TableEntry::isText() const
{
    return kind() == Kind::Text;
}

bool TableEntry::isTableSwitch() const
{
    return kind() == Kind::TableSwitch;
}

bool TableEntry::isControl() const
{
    return kind() == Kind::Control;
}

bool TableEntry::isEnd() const
{
    return kind() == Kind::End;
}

} // namespace kaizo::text