#pragma once

#include "AbstractSyntaxTree.h"
#include <fuse/assembler/symbols/SymbolTable.h>
#include <memory>

namespace fuse::assembler {

class CompilationUnit
{
public:
    auto abstractSyntaxTree() -> AbstractSyntaxTree&;
    auto symbolTable() -> SymbolTable&;

private:
    std::unique_ptr<AbstractSyntaxTree> m_ast;
    std::unique_ptr<SymbolTable> m_symbolTable;
};

} // namespace fuse::assembler