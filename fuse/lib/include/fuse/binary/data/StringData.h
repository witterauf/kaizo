#pragma once

#include "Data.h"
#include <string>

namespace kaizo::data {

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
    StringData(const StringData& other) = default;

    std::string m_value;
};

} // namespace kaizo::data