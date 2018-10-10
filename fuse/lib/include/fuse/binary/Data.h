#pragma once

#include <memory>

namespace fuse::binary {

enum class DataType
{
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
    
    virtual bool isEqual(const Data& rhs) const = 0;
    virtual auto copy() const -> std::unique_ptr<Data> = 0;

protected:
    Data(DataType type);

private:
    const DataType m_type;
};

} // namespace fuse::binary