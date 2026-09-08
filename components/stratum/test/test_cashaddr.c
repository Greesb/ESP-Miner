#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "cashaddr.h"

TEST_CASE("Cashaddr P2PKH encode", "[cashaddr]")
{
    uint8_t hash[20] = {
        0x10, 0x71, 0x4e, 0xe3, 0xf0, 0x4b, 0x34, 0xf2,
        0x4d, 0x46, 0x3a, 0xe1, 0x6c, 0x19, 0xaa, 0x94,
        0x1b, 0xf6, 0x11, 0x0b
    };
    char output[48];
    bool result = cashaddr_encode(hash, 20, P2PKH, output, 48, "ecash");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("qqg8znhr7p9nfujdgcawzmqe422phas3pv4ydym5su", output);
}

TEST_CASE("Cashaddr P2SH encode", "[cashaddr]")
{
    uint8_t hash[20] = {
        0xd3, 0x7c, 0x4c, 0x80, 0x9f, 0xe9, 0x84, 0xe,
        0x7b, 0xfa, 0x77, 0xb8, 0x6b, 0xd4, 0x71, 0x63,
        0xf6, 0xfb, 0x6c, 0x60
    };
    char output[48];

    bool result = cashaddr_encode(hash, 20, P2SH, output, 48, "ecash");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_STRING("prfhcnyqnl5cgrnmlfmms675w93ld7mvvqd0y8lz07", output);
}
