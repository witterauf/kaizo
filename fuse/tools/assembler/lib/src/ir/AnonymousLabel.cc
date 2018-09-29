#include <diagnostics/Contracts.h>
#include <fuse/assembler/ir/AnonymousLabel.h>

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
    : Label{BlockElementKind::AnonymousLabel}
{
}

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

bool AnonymousLabel::isEqual(const BlockElement& rhs) const
{
    if (rhs.kind() == BlockElementKind::AnonymousLabel)
    {
        auto const& anonymousLabelRhs = static_cast<const AnonymousLabel&>(rhs);
        return m_level == anonymousLabelRhs.m_level && m_isForward == anonymousLabelRhs.m_isForward;
    }
    return false;
}

} // namespace fuse::assembler