#include <Catch2.hpp>
#include <fuse/assembler/frontend/Classifier.h>
#include <fuse/assembler/frontend/Lexer.h>

using namespace fuse::assembler;

namespace {

struct LexerTest
{
    std::string when;
    std::string input;
    std::vector<Token> expected;
};

bool areEquivalent(const std::vector<Token>& a, const std::vector<Token>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (auto i = 0U; i < a.size(); ++i)
    {
        if (!a[i].isEquivalent(b[i]))
        {
            return false;
        }
    }
    return true;
}

const std::vector<LexerTest> SingularTokenTests = {
    {"token of kind 'Identifier'", "abc123", {Token::makeIdentifier("abc123"), Token::makeEnd()}},
    {"token of kind 'Integer'", "10", {Token::makeInteger(10), Token::makeEnd()}},
    {"token of kind 'Integer' (hexadecimal)", "$a", {Token::makeInteger(10), Token::makeEnd()}},
    {"token of kind 'Integer' (binary)", "%1010", {Token::makeInteger(10), Token::makeEnd()}},
    {"token of kind 'Dollar'", "$", {Token::makeSymbol(TokenKind::Dollar), Token::makeEnd()}},
    {"token of kind 'At'", "@", {Token::makeSymbol(TokenKind::At), Token::makeEnd()}},
    {"token of kind 'Dot'", ".", {Token::makeSymbol(TokenKind::Dot), Token::makeEnd()}},
    {"token of kind 'ParenthesisLeft'",
     "(",
     {Token::makeSymbol(TokenKind::ParenthesisLeft), Token::makeEnd()}},
    {"token of kind 'ParenthesisRight'",
     ")",
     {Token::makeSymbol(TokenKind::ParenthesisRight), Token::makeEnd()}},
    {"token of kind 'Annotation'",
     "@annotation",
     {Token::makeAnnotation("annotation"), Token::makeEnd()}},
    {"token of kind 'Directive'",
     ".directive",
     {Token::makeDirective("directive"), Token::makeEnd()}},
};

} // namespace

SCENARIO("Lexing of singular tokens without a classifier", "[Frontend][Lexing]")
{
    Lexer lexer;

    GIVEN("Singular tokens are lexed")
    {
        for (auto const& test : SingularTokenTests)
        {
            WHEN("It is a " + test.when)
            {
                auto maybeTokens = lexer.lex(test.input.c_str(), test.input.length());

                THEN("Lexing is successful")
                {
                    REQUIRE(maybeTokens.has_value());

                    AND_THEN("The returned tokens equal the expected")
                    {
                        REQUIRE(areEquivalent(*maybeTokens, test.expected));
                    }
                }
            }
        }
    }
}

namespace {

class TestClassifier : public Classifier
{
public:
    auto classify(const std::string&) const -> Classification
    {
        return classification;
    }

    Classification classification;
};

} // namespace

SCENARIO("Lexing identifiers with a classifier", "[Frontend][Lexing]")
{
    Lexer lexer;
    TestClassifier classifier;
    lexer.setClassifier(&classifier);

    GIVEN("A singular identifier")
    {
        WHEN("It is classified as an identifier")
        {
            const std::string input = "abc";
            classifier.classification = {IdentifierKind::Identifier, {}};
            auto maybeTokens = lexer.lex(input.c_str(), input.length());

            THEN("The returned token is of kind 'Identifier'")
            {
                REQUIRE(maybeTokens.has_value());
                REQUIRE(maybeTokens->size() == 2);
                REQUIRE((*maybeTokens)[0].is(TokenKind::Identifier));
            }
        }
        WHEN("It is classified as a register")
        {
            const std::string input = "abc";
            auto const RegisterId = 3ULL;
            classifier.classification = { IdentifierKind::Register, RegisterId };
            auto maybeTokens = lexer.lex(input.c_str(), input.length());

            THEN("The returned token is of kind 'Register'")
            {
                REQUIRE(maybeTokens.has_value());
                REQUIRE(maybeTokens->size() == 2);
                REQUIRE((*maybeTokens)[0].is(TokenKind::Register));

                AND_THEN("It has the classified id")
                {
                    REQUIRE((*maybeTokens)[0].registerId() == RegisterId);
                }
            }
        }
        WHEN("It is classified as a mnemonic")
        {
            const std::string input = "abc";
            auto const MnemonicId = 3ULL;
            classifier.classification = { IdentifierKind::Mnemonic, MnemonicId};
            auto maybeTokens = lexer.lex(input.c_str(), input.length());

            THEN("The returned token is of kind 'Mnemonic'")
            {
                REQUIRE(maybeTokens.has_value());
                REQUIRE(maybeTokens->size() == 3); // EndOfInstruction included
                REQUIRE((*maybeTokens)[0].is(TokenKind::Mnemonic));

                AND_THEN("It has the classified id")
                {
                    REQUIRE((*maybeTokens)[0].mnemonicId() == MnemonicId);
                }
            }
        }
        WHEN("It is classified as a custom keyword")
        {
            const std::string input = "abc";
            auto const KeywordId = 3ULL;
            classifier.classification = { IdentifierKind::Keyword, KeywordId };
            auto maybeTokens = lexer.lex(input.c_str(), input.length());

            THEN("The returned token is of kind 'CustomKeyword'")
            {
                REQUIRE(maybeTokens.has_value());
                REQUIRE(maybeTokens->size() == 2);
                REQUIRE((*maybeTokens)[0].is(TokenKind::CustomKeyword));

                AND_THEN("It has the classified id")
                {
                    REQUIRE((*maybeTokens)[0].customKeywordId() == KeywordId);
                }
            }
        }
    }
}