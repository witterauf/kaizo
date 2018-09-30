#include "TestBlockNamer.h"
#include "TestInstruction.h"
#include "TestInstructionParser.h"
#include <Catch2.hpp>
#include <fuse/assembler/frontend/Parser.h>
#include <fuse/assembler/ir/AbstractSyntaxTree.h>
#include <fuse/assembler/ir/AnonymousLabel.h>
#include <fuse/assembler/ir/Block.h>
#include <fuse/assembler/ir/IntegerLiteral.h>
#include <fuse/assembler/ir/NamedLabel.h>
#include <fuse/assembler/symbols/Symbol.h>
#include <fuse/assembler/symbols/SymbolTable.h>
#include <vector>

using namespace fuse::assembler;

namespace {

struct Fixture
{
    Fixture()
    {
        parser.setSymbolTable(&table);
        parser.setInstructionParser(&instructionParser);
        parser.setBlockNamer(&blockNamer);
    }

    Parser parser;
    SymbolTable table;
    TestInstructionParser instructionParser;
    TestBlockNamer blockNamer;
};

} // namespace

SCENARIO("Parsing annotations", "[Frontend][Parsing]")
{
    Fixture fixture;

    GIVEN("An Annotation is parsed")
    {
        WHEN("It has no argument list")
        {
            const std::vector<Token> input = {Token::makeAnnotation("abc"), Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeAnnotation = fixture.parser.parseAnnotation();

            THEN("An Annotation should be returned")
            {
                REQUIRE(maybeAnnotation.has_value());

                AND_THEN("The Annotation has no positional arguments")
                {
                    REQUIRE(maybeAnnotation.value()->positionalCount() == 0);
                }
                AND_THEN("The Annotation has no named arguments")
                {
                    REQUIRE(maybeAnnotation.value()->namedCount() == 0);
                }
            }
        }
        WHEN("It has a positional argument")
        {
            const std::vector<Token> input = {
                Token::makeAnnotation("abc"), Token::makeSymbol(TokenKind::ParenthesisLeft),
                Token::makeInteger(64), Token::makeSymbol(TokenKind::ParenthesisRight),
                Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeAnnotation = fixture.parser.parseAnnotation();

            THEN("An Annotation should be returned")
            {
                REQUIRE(maybeAnnotation.has_value());
                auto const& annotation = **maybeAnnotation;

                AND_THEN("The Annotation has a positional argument")
                {
                    REQUIRE(annotation.positionalCount() == 1);

                    AND_THEN("The argument has the expected value")
                    {
                        REQUIRE(annotation.positional(0) == 64);
                    }
                }
            }
        }
        WHEN("It has a named argument")
        {
            const std::vector<Token> input = {Token::makeAnnotation("abc"),
                                              Token::makeSymbol(TokenKind::ParenthesisLeft),
                                              Token::makeIdentifier("def"),
                                              Token::makeSymbol(TokenKind::Equal),
                                              Token::makeInteger(64),
                                              Token::makeSymbol(TokenKind::ParenthesisRight),
                                              Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeAnnotation = fixture.parser.parseAnnotation();

            THEN("An Annotation should be returned")
            {
                REQUIRE(maybeAnnotation.has_value());
                auto const& annotation = **maybeAnnotation;

                AND_THEN("The Annotation has the named argument")
                {
                    REQUIRE(annotation.namedCount() == 1);
                    REQUIRE(annotation.hasNamed("def"));

                    AND_THEN("The argument has the expected value")
                    {
                        REQUIRE(annotation.named("def") == 64);
                    }
                }
            }
        }
        WHEN("It has N >= 2 positional arguments")
        {
            const std::vector<Token> input = {Token::makeAnnotation("abc"),
                                              Token::makeSymbol(TokenKind::ParenthesisLeft),
                                              Token::makeInteger(64),
                                              Token::makeSymbol(TokenKind::Comma),
                                              Token::makeInteger(32),
                                              Token::makeSymbol(TokenKind::ParenthesisRight),
                                              Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeAnnotation = fixture.parser.parseAnnotation();

            THEN("An Annotation should be returned")
            {
                REQUIRE(maybeAnnotation.has_value());
                auto const& annotation = **maybeAnnotation;

                AND_THEN("The Annotation has N positional arguments")
                {
                    REQUIRE(annotation.positionalCount() == 2);

                    AND_THEN("The arguments have the expected value")
                    {
                        REQUIRE(annotation.positional(0) == 64);
                        REQUIRE(annotation.positional(1) == 32);
                    }
                }
            }
        }
    }
}

SCENARIO("Parsing top elements", "[Frontend][Parsing]")
{
    Fixture fixture;
    AbstractSyntaxTree ast;

    GIVEN("The next elements start with KeywordConstant")
    {
        WHEN("It is a valid constant declaration")
        {
            const std::vector<Token> input = {Token::makeKeyword(TokenKind::KeywordConstant),
                                              Token::makeIdentifier("abc"),
                                              Token::makeSymbol(TokenKind::Colon),
                                              Token::makeIdentifier("dword"),
                                              Token::makeSymbol(TokenKind::Equal),
                                              Token::makeInteger(64),
                                              Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            bool success = fixture.parser.parseTopElement(ast);

            THEN("Parsing should be successful")
            {
                REQUIRE(success);

                AND_THEN("The Symbol is registered in the SymbolTable")
                {
                    REQUIRE(fixture.table.has("abc"));

                    AND_THEN("It is a constant")
                    {
                        REQUIRE(fixture.table.lookup("abc")->isConstant());
                    }
                }
            }
        }
    }
    GIVEN("The next elements start with KeywordExtern")
    {
        WHEN("It is a valid external declaration")
        {
            const std::vector<Token> input = {Token::makeKeyword(TokenKind::KeywordExtern),
                                              Token::makeIdentifier("abc"), Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            bool success = fixture.parser.parseTopElement(ast);

            THEN("Parsing should be successful")
            {
                REQUIRE(success);

                AND_THEN("The Symbol is registered in the SymbolTable")
                {
                    REQUIRE(fixture.table.has("abc"));

                    AND_THEN("It is external")
                    {
                        REQUIRE(fixture.table.lookup("abc")->isExternal());
                    }
                }
            }
        }
    }
}

SCENARIO("Parsing BlockElements", "[Frontend][Parsing]")
{
    Fixture fixture;

    GIVEN("The next element starts with a Mnemonic")
    {
        WHEN("It is a valid Instruction")
        {
            const std::vector<Token> input = {Token::makeMnemonic(13),
                                              Token::makeEndOfInstruction(), Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeInstruction = fixture.parser.parseBlockElement();

            THEN("An Instruction is returned")
            {
                REQUIRE(maybeInstruction.has_value());
                auto const& instruction = **maybeInstruction;
                REQUIRE(instruction.isInstruction());
            }
        }
    }
    GIVEN("The next element starts with a Minus")
    {
        WHEN("It is a valid AnonymousLabel")
        {
            const std::vector<Token> input = {Token::makeSymbol(TokenKind::Minus),
                                              Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeBlockElement = fixture.parser.parseBlockElement();

            THEN("A BlockElement is returned")
            {
                REQUIRE(maybeBlockElement.has_value());

                AND_THEN("It is a backward AnonymousLabel")
                {
                    REQUIRE((*maybeBlockElement)->isAnonymousLabel());
                    auto const& label = static_cast<const AnonymousLabel&>(**maybeBlockElement);
                    REQUIRE(label.isBackward());
                }
            }
        }
    }
    GIVEN("The next element starts with a Plus")
    {
        WHEN("It is a valid AnonymousLabel")
        {
            const std::vector<Token> input = {Token::makeSymbol(TokenKind::Plus), Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeBlockElement = fixture.parser.parseBlockElement();

            THEN("A BlockElement is returned")
            {
                REQUIRE(maybeBlockElement.has_value());

                AND_THEN("It is a forward AnonymousLabel")
                {
                    REQUIRE((*maybeBlockElement)->isAnonymousLabel());
                    auto const& label = static_cast<const AnonymousLabel&>(**maybeBlockElement);
                    REQUIRE(label.isForward());
                }
            }
        }
    }
    GIVEN("The next element starts with an Identifier")
    {
        WHEN("It is a valid NamedLabel")
        {
            const std::vector<Token> input = {Token::makeIdentifier("abc"),
                                              Token::makeSymbol(TokenKind::Colon),
                                              Token::makeEnd()};
            fixture.parser.setSource(&input);
            fixture.parser.setIndex(0);
            auto maybeBlockElement = fixture.parser.parseBlockElement();

            THEN("A BlockElement is returned")
            {
                REQUIRE(maybeBlockElement.has_value());

                AND_THEN("It is a NamedLabel")
                {
                    REQUIRE((*maybeBlockElement)->isNamedLabel());
                }
            }
        }
    }
}

SCENARIO("Parsing blocks", "[Frontend][Parsing]")
{
    Parser parser;
    SymbolTable table;
    TestBlockNamer blockNamer;
    TestInstructionParser instructionParser;
    parser.setInstructionParser(&instructionParser);
    parser.setSymbolTable(&table);
    parser.setBlockNamer(&blockNamer);

    GIVEN("A Block is parsed")
    {
        WHEN("It contains no BlockElements")
        {
            const std::vector<Token> input = {Token::makeKeyword(TokenKind::KeywordBlock),
                                              Token::makeSymbol(TokenKind::CurlyBraceLeft),
                                              Token::makeSymbol(TokenKind::CurlyBraceRight)};
            parser.setSource(&input);
            parser.setIndex(0);
            auto maybeBlock = parser.parseBlock();

            THEN("A Block is returned")
            {
                REQUIRE(maybeBlock.has_value());

                AND_THEN("The Block has no BlockElements")
                {
                    REQUIRE((**maybeBlock).elementCount() == 0);
                }
                AND_THEN("The Block identifier is the last one")
                {
                    REQUIRE(maybeBlock.value()->identifier() == blockNamer.lastName());
                }
            }
        }
        WHEN("It contains n >= 1 BlockElements")
        {
            const std::vector<Token> input = {
                Token::makeKeyword(TokenKind::KeywordBlock),
                Token::makeSymbol(TokenKind::CurlyBraceLeft), Token::makeMnemonic(13),
                Token::makeEndOfInstruction(), Token::makeSymbol(TokenKind::CurlyBraceRight)};
            parser.setSource(&input);
            parser.setIndex(0);
            auto maybeBlock = parser.parseBlock();

            THEN("A Block is returned")
            {
                REQUIRE(maybeBlock.has_value());

                AND_THEN("The Block has n BlockElements")
                {
                    auto expected = std::make_unique<Block>(blockNamer.lastName());
                    REQUIRE((**maybeBlock).elementCount() == 1);
                }
            }
        }
    }
    GIVEN("A Subroutine is parsed")
    {
        const std::vector<Token> input = {Token::makeKeyword(TokenKind::KeywordSubroutine),
                                          Token::makeIdentifier("abc"),
                                          Token::makeSymbol(TokenKind::CurlyBraceLeft),
                                          Token::makeSymbol(TokenKind::CurlyBraceRight)};
        parser.setSource(&input);
        parser.setIndex(0);
        auto maybeBlock = parser.parseSubroutine();

        THEN("A Block is returned")
        {
            REQUIRE(maybeBlock.has_value());

            AND_THEN("Its identifier is the Subroutine's name")
            {
                REQUIRE(maybeBlock.value()->identifier() == "abc");
            }
        }
    }
}

SCENARIO("Parsing labels", "[Frontend][Parsing]")
{
    Parser parser;
    SymbolTable table;
    parser.setSymbolTable(&table);

    GIVEN("A NamedLabel is parsed")
    {
        const std::vector<Token> input = {Token::makeIdentifier("abc"),
                                          Token::makeSymbol(TokenKind::Colon), Token::makeEnd()};
        parser.setSource(&input);
        parser.setIndex(0);
        auto maybeLabel = parser.parseLabel();

        THEN("The label should have been added to the SymbolTable")
        {
            REQUIRE(table.has("abc"));

            AND_THEN("A NamedLabel should be returned")
            {
                REQUIRE(maybeLabel.has_value());

                AND_THEN("The AnonymousLabel should be set up as expected")
                {
                    auto expected = std::make_unique<NamedLabel>(table.lookup("abc"));
                    REQUIRE(expected->isEqual(**maybeLabel));
                }
            }
        }
    }
    GIVEN("A forward AnonymousLabel is parsed")
    {
        WHEN("Its level is 1")
        {
            const std::vector<Token> input = {Token::makeSymbol(TokenKind::Plus), Token::makeEnd()};
            parser.setSource(&input);
            parser.setIndex(0);
            auto maybeLabel = parser.parseAnonymousForwardLabel();

            THEN("An AnonymousLabel should be returned")
            {
                REQUIRE(maybeLabel.has_value());

                AND_THEN("The AnonymousLabel should be set up as expected")
                {
                    auto expected = AnonymousLabel::makeForward(1);
                    REQUIRE(expected->isEqual(**maybeLabel));
                }
            }
        }
        WHEN("Its level is 2")
        {
            const std::vector<Token> input = {Token::makeSymbol(TokenKind::Plus),
                                              Token::makeSymbol(TokenKind::Plus), Token::makeEnd()};
            parser.setSource(&input);
            parser.setIndex(0);
            auto maybeLabel = parser.parseAnonymousForwardLabel();

            THEN("An AnonymousLabel should be returned")
            {
                REQUIRE(maybeLabel.has_value());

                AND_THEN("The AnonymousLabel should be set up as expected")
                {
                    auto expected = AnonymousLabel::makeForward(2);
                    REQUIRE(expected->isEqual(**maybeLabel));
                }
            }
        }
    }
}

SCENARIO("Parsing declarations", "[Frontend][Parsing]")
{
    Parser parser;

    GIVEN("A constant declaration is parsed")
    {
        const std::vector<Token> input = {Token::makeKeyword(TokenKind::KeywordConstant),
                                          Token::makeIdentifier("abc"),
                                          Token::makeSymbol(TokenKind::Colon),
                                          Token::makeIdentifier("dword"),
                                          Token::makeSymbol(TokenKind::Equal),
                                          Token::makeInteger(64),
                                          Token::makeEnd()};
        auto const expected = Symbol::makeConstant("abc", Type::makeUnsigned(32),
                                                   std::make_unique<IntegerLiteral>(64));

        WHEN("Parsing it finishes")
        {
            parser.setSource(&input);
            parser.setIndex(0);
            auto maybeSymbol = parser.parseConstantDeclaration();

            THEN("Success")
            {
                REQUIRE(maybeSymbol.has_value());

                AND_THEN("correct")
                {
                    REQUIRE(expected->isEqual(**maybeSymbol));
                }
            }
        }
    }
    GIVEN("An external declaration is parsed")
    {
        const std::vector<Token> input = {Token::makeKeyword(TokenKind::KeywordExtern),
                                          Token::makeIdentifier("abc"), Token::makeEnd()};
        auto const expected = Symbol::makeExternal("abc");

        WHEN("Parsing it finishes")
        {
            parser.setSource(&input);
            parser.setIndex(0);
            auto maybeSymbol = parser.parseExternalDeclaration();

            THEN("Success")
            {
                REQUIRE(maybeSymbol.has_value());

                AND_THEN("correct")
                {
                    REQUIRE(expected->isEqual(**maybeSymbol));
                }
            }
        }
    }
}