#pragma once

#include <string>

namespace fuse::binary {

class Data;
class BinaryData;
class RecordData;
class StringData;
class ArrayData;

class LuaWriter
{
public:
    static auto format(const std::string& lua) -> std::string;

    auto write(const Data& data) -> std::string;
    auto write(const StringData& data) -> std::string;
    auto write(const RecordData& data) -> std::string;
    auto write(const BinaryData& data) -> std::string;
    auto write(const ArrayData& data) -> std::string;
};

} // namespace fuse::binary