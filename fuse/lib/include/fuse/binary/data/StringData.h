#pragma once

#include "Data.h"
#include <string>

namespace fuse::binary {

class StringData : public Data
{
public:
    StringData();
    explicit StringData(const std::string& value);

    void setValue(const std::string& value);
    auto value() const -> const std::string&;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    std::string m_value;
};

} // namespace fuse::binary