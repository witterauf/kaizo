#pragma once

#include "DataPath.h"
#include <map>

namespace kaizo::data::binary {

template <class T> class DataAnnotation
{
public:
    void annotate(const DataPath& path, const T& annotation)
    {
        m_annotations.insert(std::make_pair(path, annotation));
    }

    auto annotations(const DataPath& path) const -> std::vector<T>
    {
        return {};
    }

    auto begin() const { return m_annotations.begin(); }
    auto end() const { return m_annotations.end(); }
    auto cbegin() const { return m_annotations.cbegin(); }
    auto cend() const { return m_annotations.cend(); }

private:
    std::multimap<DataPath, T> m_annotations;
};

} // namespace kaizo::data::binary