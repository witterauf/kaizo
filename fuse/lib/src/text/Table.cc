#include <diagnostics/Contracts.h>
#include <fuse/text/Table.h>

namespace fuse::text {

auto TextSequence::makeText(const std::string& text) -> TextSequence
{
    Expects(!text.empty());
    TextSequence sequence;
    sequence.m_kind = Kind::Text;
    sequence.m_string = text;
    return sequence;
}

auto TextSequence::makeTableSwitch(const std::string& table) -> TextSequence
{
    Expects(!table.empty());
    TextSequence sequence;
    sequence.m_kind = Kind::TableSwitch;
    sequence.m_string = table;
    return sequence;
}

auto TextSequence::makeEnd(const Label& label) -> TextSequence
{
    Expects(!label.name.empty());
    TextSequence sequence;
    sequence.m_kind = Kind::End;
    sequence.m_label = label;
    return sequence;
}

auto TextSequence::makeControl(const Label& label, const std::vector<Parameter>& parameters)
    -> TextSequence
{
    Expects(!label.name.empty());
    TextSequence sequence;
    sequence.m_kind = Kind::Control;
    sequence.m_label = label;
    sequence.m_parameters = parameters;
    return sequence;
}

auto TextSequence::kind() const -> Kind
{
    return m_kind;
}

auto TextSequence::targetTable() const -> const std::string&
{
    Expects(kind() == Kind::TableSwitch);
    return m_string;
}

auto TextSequence::label() const -> const Label&
{
    Expects(kind() == Kind::Control || kind() == Kind::End);
    return m_label;
}

auto TextSequence::labelName() const -> const std::string&
{
    Expects(kind() == Kind::Control || kind() == Kind::End);
    return m_label.name;
}

auto TextSequence::text() const -> const std::string&
{
    Expects(kind() == Kind::Text);
    return m_string;
}

auto TextSequence::parameterCount() const -> size_t
{
    return m_parameters.size();
}

auto TextSequence::parameter(size_t index) -> Parameter&
{
    Expects(index < parameterCount());
    return m_parameters[index];
}

auto TextSequence::parameter(size_t index) const -> const Parameter&
{
    Expects(index < parameterCount());
    return m_parameters[index];
}

bool TextSequence::isText() const
{
    return kind() == Kind::Text;
}

bool TextSequence::isTableSwitch() const
{
    return kind() == Kind::TableSwitch;
}

bool TextSequence::isControl() const
{
    return kind() == Kind::Control;
}

bool TextSequence::isEnd() const
{
    return kind() == Kind::End;
}

void Table::setName(const std::string& name)
{
    m_name = name;
}

auto Table::name() const -> const std::string&
{
    return m_name;
}

bool Table::isAnonymous() const
{
    return m_name.empty();
}

auto Table::size() const -> size_t
{
    return m_mapping.size();
}

bool Table::hasControl(const std::string& label) const
{
    for (auto const& pair : m_mapping)
    {
        if (pair.second.isControl() && pair.second.labelName() == label)
        {
            return true;
        }
    }
    return false;
}

auto Table::control(const std::string& label) const -> std::optional<EntryReference>
{
    for (auto const& pair : m_mapping)
    {
        if (pair.second.isControl() && pair.second.labelName() == label)
        {
            return EntryReference{&pair.first, &pair.second};
        }
    }
    return {};
}

void Table::insert(const BinarySequence& binary, const TextSequence& text)
{
    m_mapping.insert(std::make_pair(binary, text));
}

} // namespace fuse::text
