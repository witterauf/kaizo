#include <cctype>
#include <diagnostics/Contracts.h>
#include <fuse/lua/LuaWriter.h>

namespace fuse {

bool LuaWriter::isValidIdentifier(const std::string& string)
{
    if (string.empty())
    {
        return false;
    }
    if (!std::isalnum(string[0]))
    {
        return false;
    }
    for (auto c : string)
    {
        if (std::isalnum(c))
        {
            continue;
        }
        else if (c == '_')
        {
            continue;
        }
        return false;
    }
    return true;
}

bool LuaWriter::isValidQuotedString(const std::string& string)
{
    for (auto i = 0U; i < string.length(); ++i)
    {
        if (string[i] == '\\')
        {
            if (i + 1 >= string.length())
            {
                return false;
            }
            else if (string[i + 1] != '\\')
            {
                return false;
            }
            i++;
        }
    }
    return true;
}

auto LuaWriter::escapeBrackets(const std::string& string) -> std::string
{
    return string;
}

auto LuaWriter::makeStringLiteral(const std::string& string) -> std::string
{
    if (LuaWriter::isValidQuotedString(string))
    {
        return "\"" + string + "\"";
    }
    else
    {
        return "[[" + LuaWriter::escapeBrackets(string) + "]]";
    }
}

auto LuaWriter::makeFieldName(const std::string& string) -> std::string
{
    if (LuaWriter::isValidIdentifier(string))
    {
        return string;
    }
    else
    {
        return "[" + makeStringLiteral(string) + "]";
    }
}

auto LuaWriter::start() -> LuaWriter&
{
    m_lua.clear();
    m_indentationLevel = 0;
    startTable();
    return *this;
}

auto LuaWriter::startTable() -> LuaWriter&
{
    m_lua += "{\n";
    m_indentationLevel += 1;
    enterState(State::Table);
    return *this;
}

auto LuaWriter::startConstructorTable(const std::string& name) -> LuaWriter&
{
    Expects(!name.empty());
    m_lua += name + "{\n";
    m_indentationLevel += 1;
    enterState(State::Table);
    return *this;
}

auto LuaWriter::finishTable() -> LuaWriter&
{
    Expects(isInState(State::Table));
    Expects(m_indentationLevel >= 1);
    m_indentationLevel -= 1;
    indent();
    m_lua += "}";
    leaveState();
    return *this;
}

auto LuaWriter::startField(const std::string& key) -> LuaWriter&
{
    Expects(isInState(State::Table));
    indent();
    m_lua += LuaWriter::makeFieldName(key) + " = ";
    enterState(State::Field);
    return *this;
}

auto LuaWriter::startField(size_t index) -> LuaWriter&
{
    Expects(isInState(State::Table));
    indent();
    m_lua += "[" + std::to_string(index) + "] = ";
    enterState(State::Field);
    return *this;
}

auto LuaWriter::startField() -> LuaWriter&
{
    Expects(isInState(State::Table));
    indent();
    enterState(State::Field);
    return *this;
}

auto LuaWriter::finishField() -> LuaWriter&
{
    Expects(isInState(State::Field)) m_lua += ",\n";
    leaveState();
    return *this;
}

auto LuaWriter::writeString(const std::string& string) -> LuaWriter&
{
    Expects(isInState(State::Field));
    m_lua += LuaWriter::makeStringLiteral(string);
    return *this;
}

auto LuaWriter::writeEnum(const std::string& enumName) -> LuaWriter&
{
    Expects(isInState(State::Field));
    m_lua += enumName;
    return *this;
}

auto LuaWriter::writeConstant(const std::string& constant) -> LuaWriter&
{
    Expects(isInState(State::Field));
    m_lua += constant;
    return *this;
}

auto LuaWriter::writePath(const std::filesystem::path& path) -> LuaWriter&
{
    Expects(isInState(State::Field));
    m_lua += LuaWriter::makeStringLiteral(path.string());
    return *this;
}

auto LuaWriter::writeInteger(uint64_t integer) -> LuaWriter&
{
    Expects(isInState(State::Field));
    m_lua += std::to_string(integer);
    return *this;
}

auto LuaWriter::writeInteger(int64_t integer) -> LuaWriter&
{
    Expects(isInState(State::Field));
    m_lua += std::to_string(integer);
    return *this;
}

void LuaWriter::finish()
{
    finishTable();
}

auto LuaWriter::lua() const -> std::string
{
    return m_lua;
}

void LuaWriter::indent()
{
    for (auto i = 0U; i < m_indentationLevel; ++i)
    {
        m_lua += "  ";
    }
}

bool LuaWriter::isInState(State state) const
{
    return m_stateStack.top() == state;
}

void LuaWriter::enterState(State state)
{
    m_stateStack.push(state);
}

void LuaWriter::leaveState()
{
    Expects(!m_stateStack.empty());
    m_stateStack.pop();
}

} // namespace fuse