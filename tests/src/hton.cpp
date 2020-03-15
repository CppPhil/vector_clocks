#include <cstdint>
#include <cstring>

#include <gtest/gtest.h>

#include "hton.hpp"

TEST(hton_test, test_byte_order) {
  const uint32_t i = 0xABCDF1AE;

  const auto j = vc::hton(i);

  constexpr std::array<pl::byte, 4> expected = {0xAB, 0xCD, 0xF1, 0xAE};

  std::array<pl::byte, 4> ary;
  memcpy(ary.data(), &j, sizeof(j));

  EXPECT_EQ(ary, expected);
}
