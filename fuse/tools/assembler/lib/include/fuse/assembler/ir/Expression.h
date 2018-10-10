#pragma once

namespace fuse::assembler {

enum class ExpressionKind
{
    BinaryOperation,
    IntegerLiteral,
    SymbolReference,
    UnresolvedReference
};

class Expression
{
public:
    auto kind() const -> ExpressionKind;

    virtual ~Expression() = default;
    virtual auto operandCount() const -> size_t;
    virtual auto operand(size_t index) const -> const Expression&;
    virtual auto operand(size_t index) -> Expression&;
    virtual void replace(size_t index, std::unique_ptr<Expression>&& expression);
    virtual bool isEqual(const Expression& rhs) const;

    // type, width, ...

protected:
    explicit Expression(ExpressionKind kind);

private:
    const ExpressionKind m_kind;
};

} // namespace fuse::assembler