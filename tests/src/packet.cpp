#include <cstring>

#include <gtest/gtest.h>

#include "packet.hpp"

constexpr pl::byte vstamp[40] = {
  /* pair count */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  /* actor1 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  /* actor1_clock */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
  /* actor2 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  /* actor2_clock */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};

constexpr char payload[13] = "Hello World!";

constexpr pl::byte
  buf[sizeof(uint64_t) + sizeof(vstamp) + sizeof(uint64_t) + sizeof(payload)]
  = {
    /* vstamp_size */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28,
    /* pair count */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    /* actor1 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    /* actor1_clock */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
    /* actor2 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    /* actor2_clock */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
    /* payload_size */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D,
    /* payload */
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21,
    0x00};

TEST(packet, it_should_construct) {
  const vc::packet pkt(vstamp, sizeof(vstamp), payload, sizeof(payload));

  EXPECT_EQ(0, memcmp(pkt.vstamp_buffer().data(), vstamp, sizeof(vstamp)));
  EXPECT_EQ(0, memcmp(pkt.payload_buffer().data(), payload, sizeof(payload)));
}

TEST(packet, it_should_deserialize) {
  const auto exp = vc::packet::deserialize_from_binary(buf, sizeof(buf));

  ASSERT_TRUE(exp.has_value());

  const auto pkt = *exp;

  EXPECT_EQ(0, memcmp(pkt.vstamp_buffer().data(), vstamp, sizeof(vstamp)));
  EXPECT_EQ(0, memcmp(pkt.payload_buffer().data(), payload, sizeof(payload)));
}

TEST(packet, it_should_not_deserialize_too_few_bytes) {
  const auto exp = vc::packet::deserialize_from_binary(nullptr, 15);

  ASSERT_FALSE(exp.has_value());

  EXPECT_EQ(std::string("Too few bytes were provided."),
            exp.error().message().substr(0, 28));
}

TEST(packet, it_should_not_deserialize_an_empty_timestamp) {
  pl::byte buffer[sizeof(buf)];
  memcpy(buffer, buf, sizeof(buf));

  memset(buffer, 0, sizeof(uint64_t));

  const auto exp = vc::packet::deserialize_from_binary(buffer, sizeof(buffer));

  ASSERT_FALSE(exp.has_value());

  EXPECT_EQ(std::string("A vector timestamp may not be 0 bytes wide."),
            exp.error().message().substr(0, 43));
}

TEST(packet, it_should_not_deserialize_an_empty_payload) {
  pl::byte buffer[sizeof(buf)];
  memcpy(buffer, buf, sizeof(buf));

  memset(buffer + 48, 0, sizeof(uint64_t));

  const auto exp = vc::packet::deserialize_from_binary(buffer, sizeof(buffer));

  ASSERT_FALSE(exp.has_value());

  EXPECT_EQ(std::string("A payload may not be 0 bytes wide."),
            exp.error().message().substr(0, 34));
}

TEST(packet, it_should_return_the_vstamp_buffer) {
  const vc::packet pkt(vstamp, sizeof(vstamp), payload, sizeof(payload));

  EXPECT_EQ(0, memcmp(pkt.vstamp_buffer().data(), vstamp, sizeof(vstamp)));
}

TEST(packet, it_should_return_the_payload_buffer) {
  const vc::packet pkt(vstamp, sizeof(vstamp), payload, sizeof(payload));

  EXPECT_EQ(0, memcmp(pkt.payload_buffer().data(), payload, sizeof(payload)));
}

TEST(packet, it_should_serialize_to_binary) {
  const vc::packet pkt(vstamp, sizeof(vstamp), payload, sizeof(payload));

  const auto result = pkt.serialize_to_binary();

  ASSERT_EQ(sizeof(buf), result.size());
  EXPECT_EQ(0, memcmp(result.data(), buf, sizeof(buf)));

  // roundtrip
  const auto exp = vc::packet::deserialize_from_binary(result.data(),
                                                       result.size());

  ASSERT_TRUE(exp.has_value());

  const auto deserialized_packet = *exp;

  EXPECT_EQ(0, memcmp(deserialized_packet.vstamp_buffer().data(), vstamp,
                      sizeof(vstamp)));
  EXPECT_EQ(0, memcmp(deserialized_packet.payload_buffer().data(), payload,
                      sizeof(payload)));
}
