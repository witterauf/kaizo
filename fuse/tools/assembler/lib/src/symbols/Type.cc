#include <fuse/assembler/symbols/Type.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

auto Type::makeUnsigned(size_t bitWidth) -> std::unique_ptr<Type>
{
    Expects(bitWidth > 0);
    auto type = std::make_unique<Type>();
    type->m_bitWidth = bitWidth;
    type->m_isSigned = false;
    return std::move(type);
}

auto Type::makeSigned(size_t bitWidth) -> std::unique_ptr<Type>
{
    Expects(bitWidth > 0);
    auto type = std::make_unique<Type>();
    type->m_bitWidth = bitWidth;
    type->m_isSigned = true;
    return std::move(type);
}

auto Type::bitWidth() const -> size_t
{
    return m_bitWidth;
}

bool Type::isSigned() const
{
    return m_isSigned;
}

bool Type::isUnsigned() const
{
    return !m_isSigned;
}

bool Type::isEqual(const Type& rhs) const
{
    return isSigned() == rhs.isSigned() && bitWidth() == rhs.bitWidth();
}

} // namespace fuse::assembler