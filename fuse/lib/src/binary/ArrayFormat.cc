#include <diagnostics/Contracts.h>
#include <fuse/binary/ArrayFormat.h>
#include <fuse/binary/ArrayData.h>

namespace fuse::binary {

auto ArrayFormat::doDecode(DataReader& reader) -> std::unique_ptr<Data>
{
    Expects(m_sizeProvider);
    Expects(m_elementFormat);

    auto arrayData = std::make_unique<ArrayData>();
    auto const size = m_sizeProvider->provideSize();
    for (auto i = 0U; i < size; ++i)
    {
        if (auto data = m_elementFormat->decode(reader))
        {
            arrayData->append(std::move(data));
        }
        else
        {
            return {};
        }
    }
    return std::move(arrayData);
}

} // namespace fuse::binary