#include <Catch2.hpp>
#include <fuse/assembler/frontend/Parser.h>
#include <fuse/assembler/ir/AnonymousLabel.h>
#include <fuse/assembler/ir/IntegerLiteral.h>
#include <fuse/assembler/ir/NamedLabel.h>
#include <fuse/assembler/symbols/Symbol.h>
#include <fuse/assembler/symbols/SymbolTable.h>
#include <vector>

using namespace fuse::assembler;

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