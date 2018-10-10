#pragma once

#include <memory>
#include <optional>
#include <string>

namespace fuse::binary {

class Data;
class DataReader;

class DataFormat
{
public:
    void storeAs(const std::string& label);
    void doNotStore();
    auto decode(DataReader& reader) -> std::unique_ptr<Data>;

protected:
    virtual auto doDecode(DataReader& reader) -> std::unique_ptr<Data> = 0;

private:
    std::optional<std::string> m_storeAs;
};

} // namespace fuse::binary