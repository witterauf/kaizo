#pragma once

#include "Data.h"
#include <map>
#include <string>
#include <vector>

namespace kaizo::data::binary {

class RecordData final : public Data
{
public:
    RecordData();

    void set(const std::string& name, std::unique_ptr<Data>&& data);

    auto elementCount() const -> size_t;
    bool has(const std::string& name) const;
    auto element(const std::string& name) const -> const Data&;
    auto element(const std::string& name) -> Data&;
    auto elementNames() const -> std::vector<std::string>;

    bool isEqual(const Data& rhs) const override;
    auto copy() const -> std::unique_ptr<Data> override;

private:
    RecordData(const RecordData& other);

    std::map<std::string, std::unique_ptr<Data>> m_elements;
};

} // namespace kaizo::data::binary