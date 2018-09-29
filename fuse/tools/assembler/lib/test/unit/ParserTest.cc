#include <Catch2.hpp>
#include <functional>
#include <fuse/assembler/frontend/ExpressionParser.h>
#include <fuse/assembler/frontend/Token.h>
#include <fuse/assembler/ir/BinaryOperation.h>
#include <fuse/assembler/ir/IntegerLiteral.h>

using namespace fuse::assembler;

template <class ParentBuilder, ExpressionKind Kind> struct ExpressionBuilders;

template <class ParentBuilder> class ExpressionBuilderBase
{
public:
    explicit ExpressionBuilderBase(ParentBuilder* parent)
        : m_parent{parent}
    {
    }

    auto end() -> ParentBuilder&
    {
        build();
        return parent();
    }

    virtual void build() = 0;

protected:
    auto parent() -> ParentBuilder&
    {
        return *m_parent;
    }

private:
    ParentBuilder* m_parent = nullptr;
};

template <class ParentBuilder>
class IntegerLiteralBuilder : public ExpressionBuilderBase<ParentBuilder>
{
public:
    explicit IntegerLiteralBuilder(ParentBuilder* parent)
        : ExpressionBuilderBase{parent}
    {
    }

    auto value(int64_t v) -> IntegerLiteralBuilder&
    {
        m_value = v;
        return *this;
    }

    void build() override
    {
        parent().take(std::make_unique<IntegerLiteral>(m_value));
    }

private:
    int64_t m_value{0};
};

template <class ParentBuilder>
class BinaryOperationBuilder : public ExpressionBuilderBase<ParentBuilder>
{
public:
    explicit BinaryOperationBuilder(ParentBuilder* parent)
        : ExpressionBuilderBase{parent}
    {
    }

    auto op(BinaryOperator op) -> BinaryOperationBuilder&
    {
        m_operator = op;
        return *this;
    }

    template <ExpressionKind Kind>
    auto left() -> typename ExpressionBuilders<BinaryOperationBuilder, Kind>::Type
    {
        m_buildingLeft = true;
        return typename ExpressionBuilders<BinaryOperationBuilder, Kind>::Type{this};
    }

    template <ExpressionKind Kind>
    auto right() -> typename ExpressionBuilders<BinaryOperationBuilder, Kind>::Type
    {
        m_buildingLeft = false;
        return typename ExpressionBuilders<BinaryOperationBuilder, Kind>::Type{this};
    }

    void build() override
    {
        parent().take(
            std::make_unique<BinaryOperation>(m_operator, std::move(m_left), std::move(m_right)));
    }

    void take(std::unique_ptr<Expression>&& child)
    {
        if (m_buildingLeft)
        {
            m_left = std::move(child);
        }
        else
        {
            m_right = std::move(child);
        }
    }

private:
    BinaryOperator m_operator;
    bool m_buildingLeft{true};
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
};

template <class ParentBuilder>
struct ExpressionBuilders<ParentBuilder, ExpressionKind::BinaryOperation>
{
    using Type = BinaryOperationBuilder<ParentBuilder>;
};

template <class ParentBuilder>
struct ExpressionBuilders<ParentBuilder, ExpressionKind::IntegerLiteral>
{
    using Type = IntegerLiteralBuilder<ParentBuilder>;
};

class ExpressionBuilder
{
public:
    template <ExpressionKind Kind>
    auto build() -> typename ExpressionBuilders<ExpressionBuilder, Kind>::Type
    {
        return typename ExpressionBuilders<ExpressionBuilder, Kind>::Type{this};
    }

    void take(std::unique_ptr<Expression>&& child)
    {
        m_expression = std::move(child);
    }

    auto end() -> std::unique_ptr<Expression>
    {
        return std::move(m_expression);
    }

private:
    std::unique_ptr<Expression> m_expression;
};

struct ParserTest
{
    std::string when;
    std::vector<Token> input;
    std::function<std::unique_ptr<Expression>()> makeExpected;
};

const std::vector<ParserTest> FactorTests = {
    {"'Integer'", {Token::makeInteger(13)}, []() { return std::make_unique<IntegerLiteral>(13); }},
    {"'Integer' in parentheses",
     {Token::makeSymbol(TokenKind::ParenthesisLeft), Token::makeInteger(11),
      Token::makeSymbol(TokenKind::ParenthesisRight)},
     []() { return std::make_unique<IntegerLiteral>(11); }},
};

const std::vector<ParserTest> TermTests = {
    {"single product",
     {Token::makeInteger(13), Token::makeSymbol(TokenKind::Asterisk), Token::makeInteger(24)},
     []() {
         // clang-format off
        return ExpressionBuilder{}.build<ExpressionKind::BinaryOperation>()
            .op(BinaryOperator::Multiply)
                .left<ExpressionKind::IntegerLiteral>().value(13).end()
                .right<ExpressionKind::IntegerLiteral>().value(24).end()
                .end()
            .end();
         // clang-format on
     }},
};

const std::vector<ParserTest> SimpleExpressionTests = {
    {"single addition",
     {Token::makeInteger(13), Token::makeSymbol(TokenKind::Plus), Token::makeInteger(24)},
     []() {
         // clang-format off
        return ExpressionBuilder{}.build<ExpressionKind::BinaryOperation>()
            .op(BinaryOperator::Add)
                .left<ExpressionKind::IntegerLiteral>().value(13).end()
                .right<ExpressionKind::IntegerLiteral>().value(24).end()
                .end()
            .end();
         // clang-format on
     }},
    {"addition and subtraction",
     {Token::makeInteger(13), Token::makeSymbol(TokenKind::Plus), Token::makeInteger(24),
      Token::makeSymbol(TokenKind::Minus), Token::makeInteger(17)},
     []() {
         // clang-format off
        return ExpressionBuilder{}.build<ExpressionKind::BinaryOperation>()
            .op(BinaryOperator::Subtract)
                .left<ExpressionKind::BinaryOperation>()
                    .op(BinaryOperator::Add)
                        .left<ExpressionKind::IntegerLiteral>().value(13).end()
                        .right<ExpressionKind::IntegerLiteral>().value(24).end()
                    .end()
                .right<ExpressionKind::IntegerLiteral>().value(17).end()
                .end()
            .end();
         // clang-format on
     }},
    {"addition of complex Terms",
     {Token::makeInteger(13), Token::makeSymbol(TokenKind::Plus), Token::makeInteger(24),
      Token::makeSymbol(TokenKind::Asterisk), Token::makeInteger(17)},
     []() {
         // clang-format off
        return ExpressionBuilder{}.build<ExpressionKind::BinaryOperation>()
            .op(BinaryOperator::Add)
                .left<ExpressionKind::IntegerLiteral>().value(13).end()
                .right<ExpressionKind::BinaryOperation>()
                    .op(BinaryOperator::Multiply)
                        .left<ExpressionKind::IntegerLiteral>().value(24).end()
                        .right<ExpressionKind::IntegerLiteral>().value(17).end()
                    .end()
                .end()
            .end();
         // clang-format on
     }},
};

struct ParserTestSuite
{
    std::string given;
    const std::vector<ParserTest>& tests;
};

const std::vector<ParserTestSuite> TestSuites = {
    {"Factor", FactorTests}, {"TermTests", TermTests}, {"SimpleExpression", SimpleExpressionTests}};

SCENARIO("Parsing singular tokens", "[Frontend][Parsing]")
{
    ExpressionParser parser;

    for (auto const& suite : TestSuites)
    {
        GIVEN("Input containing a " + suite.given + " is parsed")
        {
            for (auto const& test : suite.tests)
            {
                WHEN("Parsing a singular token of kind " + test.when)
                {
                    auto input = test.input;
                    input.push_back(Token::makeEnd());
                    auto maybeExpression = parser.parse(input, 0);

                    THEN("An Expression should be returned")
                    {
                        REQUIRE(maybeExpression.has_value());

                        AND_THEN("The Expression should match the expected")
                        {
                            auto expected = test.makeExpected();
                            REQUIRE(expected->isEqual(**maybeExpression));
                        }
                    }
                }
            }
        }
    }
}