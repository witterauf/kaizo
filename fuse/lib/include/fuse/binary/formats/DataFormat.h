#pragma once

#include <memory>
#include <optional>
#include <string>

namespace fuse::binary {

class Data;
class DataReader;
class DataWriter;

class DataFormat
{
public:
    void storeAs(const std::string& label);
    void doNotStore();

    void setAlignment(size_t alignment);
    void setSkipAfter(size_t size);
    void setSkipBefore(size_t size);
    auto decode(DataReader& reader) -> std::unique_ptr<Data>;
    void encode(DataWriter& writer, const Data& data);
    virtual bool isPointer() const { return false; }

    template <class T> auto copyAs() -> std::unique_ptr<T>;
    virtual auto copy() const -> std::unique_ptr<DataFormat> = 0;

protected:
    virtual auto doDecode(DataReader& reader) -> std::unique_ptr<Data> = 0;
    virtual void doEncode(DataWriter& writer, const Data& data) = 0;
    void copyDataFormat(DataFormat& format) const;

private:
    size_t m_alignment{1};
    size_t m_skipBefore{0};
    size_t m_skipAfter{0};
    std::optional<std::string> m_storeAs;
};

template <class T> auto DataFormat::copyAs() -> std::unique_ptr<T>
{
    auto copied = copy();
    return std::unique_ptr<T>(static_cast<T*>(copied.release()));
}

} // namespace fuse::binary