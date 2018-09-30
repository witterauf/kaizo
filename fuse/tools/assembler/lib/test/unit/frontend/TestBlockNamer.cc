#include "TestBlockNamer.h"
#include <diagnostics/Contracts.h>

auto TestBlockNamer::generateName() -> std::string
{
    return "block" + std::to_string(m_index++);
}

auto TestBlockNamer::lastName(size_t offset) const -> std::string
{
    Expects(m_index > 0);
    Expects(m_index > offset);
    return "block" + std::to_string(m_index - offset - 1);
}
