#include <Catch2.hpp>
#include <fuse/link/SegmentEncoding.h>

using namespace fuse::link;

SCENARIO("Validation of SegmentEncoding", "[Link][SegmentEncoding]")
{
    GIVEN("The validity of a SegmentEncoding is checked")
    {
        WHEN("The SegmentEncoding was default-constructed")
        {
            SegmentEncoding encoding;

            THEN("It should be invalid")
            {
                REQUIRE_FALSE(encoding.isValid());
            }
        }
        WHEN("The SegmentEncoding was not default-constructed with a width > 0")
        {
            SegmentEncoding encoding{8, 16};

            THEN("It should be valid")
            {
                REQUIRE(encoding.isValid());
            }
        }
    }
}

SCENARIO("Segment <-> address conversion with SegmentEncoding", "[Link][SegmentEncoding]")
{
    GIVEN("A segment is extracted from an address")
    {
        SegmentEncoding encoding{8, 16};

        THEN("The correct segment is returned")
        {
            REQUIRE(encoding.toSegment(0xbcabde) == 0xbc);
        }
    }
    GIVEN("An address is constructed from a segment")
    {
        SegmentEncoding encoding{8, 16};

        THEN("The correct address is returned")
        {
            REQUIRE(encoding.toAddress(0xbc) == 0xbc0000);
        }
    }
}