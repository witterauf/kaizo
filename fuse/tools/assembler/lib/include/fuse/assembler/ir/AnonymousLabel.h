#pragma once

#include "Label.h"
#include <cstddef>

namespace fuse::assembler {

class AnonymousLabel : public Label
{
public:
    static auto makeForward(size_t level) -> std::unique_ptr<AnonymousLabel>;
    static auto makeBackward(size_t level) -> std::unique_ptr<AnonymousLabel>;

    AnonymousLabel();

    bool isBackward() const;
    bool isForward() const;
    auto level() const -> size_t;

    bool isEqual(const BlockElement& rhs) const override;

private:
    bool m_isForward{true};
    size_t m_level{0};
};

} // namespace fuse::assembler