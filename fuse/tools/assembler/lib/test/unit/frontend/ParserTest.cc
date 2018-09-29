#include <Catch2.hpp>
#include <fuse/assembler/frontend/Parser.h>
#include <fuse/assembler/ir/IntegerLiteral.h>
#include <fuse/assembler/symbols/Symbol.h>
#include <vector>

using namespace fuse::assembler;

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
}