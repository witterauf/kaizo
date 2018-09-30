#pragma once

#include <fuse/assembler/frontend/BlockNamer.h>
#include <cstddef>

class TestBlockNamer : public fuse::assembler::BlockNamer
{
public:
    auto generateName() -> std::string override;
    auto lastName(size_t offset = 0) const -> std::string;

private:
    size_t m_index{0};
};