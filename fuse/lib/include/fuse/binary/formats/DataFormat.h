#pragma once

#include <memory>
#include <optional>
#include <string>

namespace kaizo::data {

class Data;
class DataReader;
class DataWriter;

class DataFormat
{
public:
    virtual ~DataFormat() = default;

    void setFixedOffset(size_t offset);
    void setAlignment(size_t alignment);
    void setSkipAfter(size_t size);
    void setSkipBefore(size_t size);
    bool hasTag() const;
    auto tag() const -> const std::string&;
    void setTag(const std::string& tag);
    auto decode(DataReader& reader) -> std::unique_ptr<Data>;
    void encode(DataWriter& writer, const Data& data);
    virtual bool isPointer() const { return false; }

    template <class T> auto copyAs() -> std::unique_ptr<T>;
    virtual auto copy() const -> std::unique_ptr<DataFormat> = 0;

protected:
    DataFormat() = default;
    DataFormat(const DataFormat& other) = default;

    virtual auto doDecode(DataReader& reader) -> std::unique_ptr<Data> = 0;
    virtual void doEncode(DataWriter& writer, const Data& data) = 0;
    void track(DataReader& reader, size_t offset, size_t size);

private:
    std::optional<size_t> m_offset;
    size_t m_alignment{1};
    size_t m_skipBefore{0};
    size_t m_skipAfter{0};
    std::optional<std::string> m_storeAs;
    std::optional<std::string> m_trackTag;
};

template <class T> auto DataFormat::copyAs() -> std::unique_ptr<T>
{
    auto copied = copy();
    return std::unique_ptr<T>(static_cast<T*>(copied.release()));
}

} // namespace kaizo::data