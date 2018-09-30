#include <diagnostics/Contracts.h>
#include <fuse/assembler/ir/Block.h>

namespace fuse::assembler {

Block::Block(const std::string& identifier)
    : m_identifier{identifier}
{
}

void Block::append(std::unique_ptr<BlockElement>&& element)
{
    Expects(element);
    m_elements.push_back(std::move(element));
}

auto Block::elementCount() const -> size_t
{
    return m_elements.size();
}

auto Block::element(size_t index) const -> const BlockElement&
{
    Expects(index < elementCount());
    return *m_elements[index];
}

auto Block::identifier() const -> const std::string&
{
    return m_identifier;
}

bool Block::isEqual(const Block& rhs) const
{
    if (rhs.elementCount() != elementCount())
    {
        return false;
    }
    for (auto i = 0U; i < elementCount(); ++i)
    {
        if (!element(i).isEqual(rhs.element(i)))
        {
            return false;
        }
    }
    return true;
}

} // namespace fuse::assembler