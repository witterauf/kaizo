#pragma once

#include <string>

namespace fuse::binary {

class Data;
class BinaryData;
class RecordData;
class StringData;

class LuaWriter
{
public:
    auto write(const Data& data) -> std::string;
    auto write(const StringData& data) -> std::string;
    auto write(const RecordData& data) -> std::string;
    auto write(const BinaryData& data) -> std::string;
};

} // namespace fuse::binary