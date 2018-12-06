#include <diagnostics/Contracts.h>
#include <fuse/binary/data/ArrayData.h>

namespace fuse::binary {

ArrayData::ArrayData(size_t size)
    : Data{DataType::Array}
    , m_elements(size)
{
}

ArrayData::ArrayData()
    : Data{DataType::Array}
{
}

void ArrayData::set(size_t index, std::unique_ptr<Data>&& element)
{
    if (index >= m_elements.size())
    {
        m_elements.resize(index + 1);
    }
    m_elements[index] = std::move(element);
}

bool ArrayData::hasElement(size_t index) const
{
    return index < m_elements.size() && m_elements[index] != nullptr;
}

void ArrayData::append(std::unique_ptr<Data>&& element)
{
    m_elements.push_back(std::move(element));
}

auto ArrayData::elementCount() const -> size_t
{
    return m_elements.size();
}

auto ArrayData::element(size_t index) const -> const Data&
{
    Expects(index < elementCount());
    return *m_elements[index];
}

auto ArrayData::element(size_t index) -> Data&
{
    Expects(index < elementCount());
    return *m_elements[index];
}

bool ArrayData::isEqual(const Data& rhs) const
{
    if (rhs.type() == type())
    {
        auto const& arrayRhs = static_cast<const ArrayData&>(rhs);
        if (elementCount() != arrayRhs.elementCount())
        {
            return false;
        }
        for (auto i = 0U; i < elementCount(); ++i)
        {
            if (!element(i).isEqual(arrayRhs.element(i)))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

auto ArrayData::copy() const -> std::unique_ptr<Data>
{
    auto data = std::make_unique<ArrayData>();
    for (auto i = 0U; i < elementCount(); ++i)
    {
        data->append(element(i).copy());
    }
    return std::move(data);
}

} // namespace fuse::binary