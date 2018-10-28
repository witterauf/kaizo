#include <fuse/assembler/ir/AbstractSyntaxTree.h>
#include <fuse/assembler/ir/Block.h>
#include <fuse/assembler/ir/BlockElement.h>
#include <fuse/assembler/ir/CompilationUnit.h>
#include <fuse/assembler/ir/ExpressionOperand.h>
#include <fuse/assembler/ir/Instruction.h>
#include <fuse/assembler/ir/InstructionOperand.h>
#include <fuse/assembler/ir/SymbolReference.h>
#include <fuse/assembler/ir/UnresolvedReference.h>
#include <fuse/assembler/transformations/SymbolResolver.h>

namespace fuse::assembler {

bool SymbolResolver::transform(CompilationUnit& unit)
{
    m_unit = &unit;
    bool success{true};
    auto& ast = unit.abstractSyntaxTree();

    for (auto i = 0U; i < ast.blockCount(); ++i)
    {
        auto& block = ast.block(i);
        for (auto e = 0U; e < block.elementCount(); ++e)
        {
            auto& blockElement = block.element(e);
            if (!transform(blockElement))
            {
                success = false;
            }
        }
    }
    return success;
}

bool SymbolResolver::transform(BlockElement& element)
{
    switch (element.kind())
    {
    case BlockElementKind::Instruction: return transform(static_cast<Instruction&>(element));
    default: return true;
    }
}

bool SymbolResolver::transform(Instruction& instruction)
{
    for (auto i = 0U; i < instruction.operandCount(); ++i)
    {
        switch (instruction.operand(i).kind())
        {
        case InstructionOperandKind::AddressExpression:
        case InstructionOperandKind::ValueExpression:
            return transform(static_cast<ExpressionOperand&>(instruction.operand(i)).expression());

        default: continue;
        }
    }
}

bool SymbolResolver::transform(Expression& expression)
{
    for (auto i = 0U; i < expression.operandCount(); ++i)
    {
        switch (expression.operand(i).kind())
        {
        case ExpressionKind::UnresolvedReference: return resolve(expression, i);
        default: transform(expression.operand(i));
        }
    }
}

bool SymbolResolver::resolve(Expression& expression, size_t index)
{
    auto& unresolved = static_cast<UnresolvedReference&>(expression.operand(index));
    auto& symbolTable = m_unit->symbolTable();

    if (symbolTable.has(unresolved.identifier()))
    {
        auto* symbol = symbolTable.lookup(unresolved.identifier());
        auto symbolReference = std::make_unique<SymbolReference>(symbol);
        expression.replace(index, std::move(symbolReference));
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace fuse::assembler