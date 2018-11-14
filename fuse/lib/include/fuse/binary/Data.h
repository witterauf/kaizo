#pragma once

#include <memory>

namespace fuse::binary {

enum class DataType
{
    Null,
    Array,
    Record,
    Integer,
    String,
    Binary,
    Image,
    Custom
};

class Data
{
public:
    virtual ~Data() = default;
    auto type() const -> DataType;

    template <class T> auto copyAs() const -> std::unique_ptr<T>;

    virtual bool isEqual(const Data& rhs) const = 0;
    virtual auto copy() const -> std::unique_ptr<Data> = 0;

protected:
    Data(DataType type);

private:
    const DataType m_type;
};

template <class T> auto Data::copyAs() const -> std::unique_ptr<T>
{
    auto copied = copy();
    return std::unique_ptr<T>(static_cast<T*>(copied.release()));
}

} // namespace fuse::binary