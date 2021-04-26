#include "kaizo/text/TableDecoder.h"
#include <algorithm>
#include <contracts/Contracts.h>
#include <kaizo/utilities/StringAlgorithms.h>

namespace kaizo {

TableDecoder::TableDecoder(const Table& table)
{
    m_tables.push_back(table);
    m_activeTable = 0;
}

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

void TableDecoder::addTable(const Table& table)
{
    Expects(!hasTable(table.name()));
    m_tables.push_back(table);
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

void TableDecoder::addHook(const std::string& name, HookHandler* hook)
{
    m_hooks[name] = hook;
}

auto TableDecoder::decode(const BinaryView& binary, size_t offset) -> std::pair<size_t, std::string>
{
    m_binary = &binary;
    m_offset = offset;

    std::string text;
    bool finished{false};
    while (!finished)
    {
        if (auto maybeMatch = activeTable().findLongestBinaryMatch(data(), binary.end()))
        {
            switch (maybeMatch->text().kind())
            {
            case TableEntry::Kind::Text:
                advance(maybeMatch->binary().size());
                text += decodeText(maybeMatch->text());
                break;
            case TableEntry::Kind::End:
                advance(maybeMatch->binary().size());
                text += decodeEnd(maybeMatch->text());
                finished = true;
                break;
            case TableEntry::Kind::TableSwitch:
                advance(maybeMatch->binary().size());
                text += decodeTableSwitch(maybeMatch->text());
                break;
            case TableEntry::Kind::Control:
                advance(maybeMatch->binary().size());
                text += decodeControl(maybeMatch->text());
                break;
            case TableEntry::Kind::Hook: text += decodeHook(maybeMatch->text()); break;
            default: throw std::runtime_error{"invalid case"};
            }
        }
        else
        {
            throw std::runtime_error{"offset " + toString(m_offset, 16, 8) + ": no match for 0x" +
                                     toString(binary[m_offset], 16, 2) + " in table"};
        }

        if (m_fixedLength)
        {
            auto const length = m_offset - offset;
            if (length == *m_fixedLength)
            {
                finished = true;
            }
            else if (length > *m_fixedLength)
            {
                throw std::runtime_error{"exceeded fixed length"};
            }
        }
    }
    if (m_fixedLength)
    {
        auto const length = m_offset - offset;
        if (length < *m_fixedLength)
        {
            m_offset = offset + length;
        }
    }
    return std::make_pair(m_offset, text);
}

auto TableDecoder::decodeControl(const TableEntry& control) -> std::string
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
            text += decodeArgument(control.parameter(i));
        }
    }
    text += "}";
    text += control.label().postfix;
    return text;
}

auto TableDecoder::decodeArgument(const TableEntry::ParameterFormat& format) -> std::string
{
    auto const argument = static_cast<uint64_t>(format.decode(data()));
    advance(format.size);

    switch (format.preferedDisplay)
    {
    case TableEntry::ParameterFormat::Display::Decimal: return std::to_string(argument);
    case TableEntry::ParameterFormat::Display::Hexadecimal: return "0x" + toString(argument, 16);
    case TableEntry::ParameterFormat::Display::Binary: return "0b" + toString(argument, 2);
    default: InvalidCase(format.preferedDisplay);
    }
}

auto TableDecoder::decodeText(const TableEntry& text) -> std::string
{
    return text.text();
}

auto TableDecoder::decodeEnd(const TableEntry& end) -> std::string
{
    std::string text = "{" + end.labelName() + "}";
    return text;
}

auto TableDecoder::decodeTableSwitch(const TableEntry& tableSwitch) -> std::string
{
    if (!hasTable(tableSwitch.targetTable()))
    {
        throw std::runtime_error{"no such table: " + tableSwitch.targetTable()};
    }
    setActiveTable(tableSwitch.targetTable());
    return {};
}

auto TableDecoder::decodeHook(const TableEntry& hook) -> std::string
{
    auto const iter = m_hooks.find(hook.hook());
    if (iter != m_hooks.cend())
    {
        if (auto const maybeResult = iter->second->decode(*m_binary, m_offset))
        {
            auto const [offset, args] = *maybeResult;
            m_offset = offset;
            std::string text = "{" + hook.hook();
            text += ":";
            text += args;
            text += "}";
            return text;
        }
        else
        {
            throw std::runtime_error{"offset " + toString(m_offset, 16, 8) + ": hook '" +
                                     hook.hook() + "' failed"};
        }
    }
    else
    {
        throw std::runtime_error{"offset " + toString(m_offset, 16, 8) + ": no handler for hook '" +
                                 hook.hook() + "' installed"};
    }
}

auto TableDecoder::data() const -> const uint8_t*
{
    return m_binary->data() + m_offset;
}

void TableDecoder::advance(size_t size)
{
    m_offset += size;
}

} // namespace kaizo