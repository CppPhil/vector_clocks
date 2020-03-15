#include <cstdint>
#include <cstring>

#include <array>

#include <gtest/gtest.h>

#include "ntoh.hpp"

TEST(ntoh_test, test_byte_order)
{
    constexpr std::array<pl::byte, 4> ary = {
        0x7F, 0x12, 0xAF, 0xFE
    };

    uint32_t i;
    static_assert(ary.size() == sizeof(i), "uint32_t isn't 4 bytes wide.");
    memcpy(&i, ary.data(), sizeof(i));

    const auto j = vc::ntoh(i);

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    std::array<pl::byte, 4> array;
    memcpy(array.data(), &j, sizeof(j));

    EXPECT_EQ(array[0], 0xFE);
    EXPECT_EQ(array[1], 0xAF);
    EXPECT_EQ(array[2], 0x12);
    EXPECT_EQ(array[3], 0x7F);
#elif Q_BYTE_ORDER == Q_BIG_ENDIAN
    EXPECT_EQ(memcmp(&j, &i, sizeof(i)), 0);
#else
#error "Unsupported byte order"
#endif
}

