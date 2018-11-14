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

    void setAlignment(size_t alignment);
    auto decode(DataReader& reader) -> std::unique_ptr<Data>;

    virtual auto copy() const -> std::unique_ptr<DataFormat> = 0;

protected:
    virtual auto doDecode(DataReader& reader) -> std::unique_ptr<Data> = 0;
    void copyTo(DataFormat& format) const;

private:
    size_t m_alignment{1};
    std::optional<std::string> m_storeAs;
};

} // namespace fuse::binary