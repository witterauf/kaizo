#include <fuse/assembler/ir/AnonymousLabel.h>
#include <diagnostics/Contracts.h>

namespace fuse::assembler {

auto AnonymousLabel::makeForward(size_t level) -> std::unique_ptr<AnonymousLabel>
{
    Expects(level > 0);
    auto label = std::make_unique<AnonymousLabel>();
    label->m_isForward = true;
    label->m_level = level;
    return std::move(label);
}

auto AnonymousLabel::makeBackward(size_t level) -> std::unique_ptr<AnonymousLabel>
{
    Expects(level > 0);
    auto label = std::make_unique<AnonymousLabel>();
    label->m_isForward = false;
    label->m_level = level;
    return std::move(label);
}

AnonymousLabel::AnonymousLabel()
    : Label{BlockElementKind::AnonymousLabel} {}

bool AnonymousLabel::isBackward() const
{
    return !m_isForward;
}

bool AnonymousLabel::isForward() const
{
    return m_isForward;
}

auto AnonymousLabel::level() const -> size_t
{
    return m_level;
}

} // namespace fuse::assembler