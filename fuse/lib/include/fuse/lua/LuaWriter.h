#pragma once

#include <filesystem>
#include <stack>
#include <string>

namespace fuse {

class LuaWriter
{
public:
    static bool isValidIdentifier(const std::string& string);
    static bool isValidQuotedString(const std::string& string);
    static auto escapeBrackets(const std::string& string) -> std::string;
    static auto makeStringLiteral(const std::string& string) -> std::string;
    static auto makeFieldName(const std::string& string) -> std::string;

    auto start() -> LuaWriter&;
    auto startTable() -> LuaWriter&;
    auto startConstructorTable(const std::string& name) -> LuaWriter&;
    auto finishTable() -> LuaWriter&;
    auto startField(const std::string& key) -> LuaWriter&;
    auto startField(size_t index) -> LuaWriter&;
    auto startField() -> LuaWriter&;
    auto finishField() -> LuaWriter&;
    auto writeString(const std::string& name) -> LuaWriter&;
    auto writeConstant(const std::string& constant) -> LuaWriter&;
    auto writeEnum(const std::string& enumName) -> LuaWriter&;
    auto writePath(const std::filesystem::path& path) -> LuaWriter&;
    auto writeInteger(uint64_t integer) -> LuaWriter&;
    auto writeInteger(int64_t integer) -> LuaWriter&;
    void finish();

    auto lua() const -> std::string;

private:
    void indent();

    enum class State
    {
        Table,
        Field
    };

    bool isInState(State state) const;
    void enterState(State state);
    void leaveState();

    std::string m_lua;
    size_t m_indentationLevel{0};
    std::stack<State> m_stateStack;
};

} // namespace fuse