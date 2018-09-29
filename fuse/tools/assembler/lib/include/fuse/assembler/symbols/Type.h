#pragma once

#include <cstddef>
#include <memory>

namespace fuse::assembler {

class Type
{
public:
    static auto makeUnsigned(size_t bitWidth) -> std::unique_ptr<Type>;
    static auto makeSigned(size_t bitWidth) -> std::unique_ptr<Type>;

    auto bitWidth() const -> size_t;
    bool isSigned() const;
    bool isUnsigned() const;

    bool isEqual(const Type& rhs) const;

private:
    bool m_isSigned{false};
    size_t m_bitWidth{0};
};

} // namespace fuse::assembler