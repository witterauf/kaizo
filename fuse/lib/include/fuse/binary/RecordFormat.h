#pragma once

#include "DataFormat.h"
#include <string>
#include <vector>

namespace fuse::binary {

class RecordFormat : public DataFormat
{
public:
    struct Element
    {
        std::string name;
        std::unique_ptr<DataFormat> format;
    };

    void append(const std::string& name, std::unique_ptr<DataFormat>&& data);

    auto elementCount() const -> size_t;
    auto has(const std::string& name);
    auto element(size_t index) const -> const Element&;
    auto element(const std::string& name) -> const Element&;

protected:
    auto doDecode(DataReader& reader)->std::unique_ptr<Data> override;

private:
    std::vector<Element> m_elements;
};

} // namespace fuse::binary