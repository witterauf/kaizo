#pragma once

#include "Serialization.h"
#include <sol.hpp>
#include <string>

namespace fuse {

namespace binary {
class BinaryData;
class RecordData;
class StringData;
class ArrayData;
class IntegerData;
class NullData;
} // namespace binary

class LuaSerialization : public DataSerialization
{
public:
    static auto format(const std::string& lua) -> std::string;

    explicit LuaSerialization(sol::this_state state);

    void serialize(const binary::Data& data, const std::filesystem::path& filename) override;
    auto deserialize(const std::filesystem::path& filename)
        -> std::unique_ptr<binary::Data> override;

    auto write(const binary::Data& data) -> std::string;
    auto write(const binary::NullData& data) -> std::string;
    auto write(const binary::StringData& data) -> std::string;
    auto write(const binary::RecordData& data) -> std::string;
    auto write(const binary::BinaryData& data) -> std::string;
    auto write(const binary::ArrayData& data) -> std::string;
    auto write(const binary::IntegerData& data) -> std::string;

    auto read(const sol::object& object) -> std::unique_ptr<binary::Data>;
    auto readString(const sol::object& object) -> std::unique_ptr<binary::Data>;
    auto readComplex(const sol::object& object) -> std::unique_ptr<binary::Data>;
    auto readArray(const sol::table& table) -> std::unique_ptr<binary::Data>;
    auto readRecord(const sol::table& table) -> std::unique_ptr<binary::Data>;
    auto readInteger(const sol::object& object) -> std::unique_ptr<binary::Data>;

private:
    sol::this_state m_state;
};

} // namespace fuse