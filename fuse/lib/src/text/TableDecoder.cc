#include <algorithm>
#include <diagnostics/Contracts.h>
#include <fuse/text/TableDecoder.h>

namespace fuse::text {

auto TableDecoder::tableCount() const -> size_t
{
    return m_tables.size();
}

bool TableDecoder::hasTable(const std::string& name) const
{
    return m_tables.cend() !=
           std::find_if(m_tables.cbegin(), m_tables.cend(),
                        [&name](auto const& table) { return table.name() == name; });
}

auto TableDecoder::activeTable() const -> const Table&
{
    return m_tables[m_activeTable];
}

void TableDecoder::addTable(Table&& table)
{
    Expects(!hasTable(table.name()));
    m_tables.push_back(std::move(table));
}

void TableDecoder::setActiveTable(size_t index)
{
    Expects(index < tableCount());
    m_activeTable = index;
}

void TableDecoder::setActiveTable(const std::string& name)
{
    Expects(hasTable(name));
    for (auto i = 0U; i < tableCount(); ++i)
    {
        if (m_tables[i].name() == name)
        {
            m_activeTable = i;
            return;
        }
    }
}

void TableDecoder::setFixedLength(size_t length)
{
    m_fixedLength = length;
}

void TableDecoder::unsetFixedLength()
{
    m_fixedLength = {};
}

auto TableDecoder::decode(const Binary& binary, size_t offset) -> std::pair<size_t, std::string>
{
    std::string text;
    size_t length{0};
    bool finished{false};
    while (!finished)
    {
        if (auto maybeMatch =
                activeTable().findLongestBinaryMatch(binary.data(offset + length), binary.end()))
        {
            length += maybeMatch->binary().size();
            switch (maybeMatch->text().kind())
            {
            case TextSequence::Kind::Text: text += decodeText(maybeMatch->text()); break;
            case TextSequence::Kind::End:
                text += decodeEnd(maybeMatch->text());
                finished = true;
                break;
            case TextSequence::Kind::TableSwitch:
                text += decodeTableSwitch(maybeMatch->text());
                break;
            case TextSequence::Kind::Control: text += decodeControl(maybeMatch->text()); break;
            }
        }
        else
        {
            text += "{" + std::to_string(*binary.data(offset + length)) + "}";
            length++;
        }

        if (m_fixedLength)
        {
            if (*m_fixedLength == length)
            {
                finished = true;
            }
            else if (length > *m_fixedLength)
            {
                throw std::runtime_error{"exceeded fixed length"};
            }
        }
    }
    return std::make_pair(offset + length, text);
}

static auto appendLineBreaks(std::string text, size_t count) -> std::string
{
    for (auto i = 0U; i < count; ++i)
    {
        text += '\n';
    }
    return text;
}

auto TableDecoder::decodeControl(const TextSequence& control) -> std::string
{
    std::string text = "{" + control.labelName();
    if (control.parameterCount() > 0)
    {
        text += ":";
        for (auto i = 0U; i < control.parameterCount(); ++i)
        {
            if (i > 0)
            {
                text += ",";
            }
        }
    }
    text += "}";
    text = appendLineBreaks(text, control.label().lineBreaks);
    return text;
}

auto TableDecoder::decodeText(const TextSequence& text) -> std::string
{
    return text.text();
}

auto TableDecoder::decodeEnd(const TextSequence& end) -> std::string
{
    std::string text = "{" + end.labelName() + "}";
    return text;
}

auto TableDecoder::decodeTableSwitch(const TextSequence& tableSwitch) -> std::string
{
    if (!hasTable(tableSwitch.targetTable()))
    {
        throw std::runtime_error{"no such table: " + tableSwitch.targetTable()};
    }
    setActiveTable(tableSwitch.targetTable());
    return {};
}

} // namespace fuse::text