#pragma once

#include "DataFormat.h"
#include <string>
#include <vector>

namespace kaizo::data {

class RecordFormat final : public DataFormat
{
public:
    struct Element
    {
        std::string name;
        std::unique_ptr<DataFormat> format;
    };

    RecordFormat() = default;

    void append(const std::string& name, std::unique_ptr<DataFormat>&& data);

    auto elementCount() const -> size_t;
    auto has(const std::string& name);
    auto element(size_t index) const -> const Element&;
    auto element(const std::string& name) -> const Element&;

    auto copy() const -> std::unique_ptr<DataFormat> override;

protected:
    RecordFormat(const RecordFormat& other);

    auto doDecode(DataReader& reader) -> std::unique_ptr<Data> override;
    void doEncode(DataWriter& writer, const Data& data) override;

private:
    std::vector<Element> m_elements;
};

} // namespace kaizo::data