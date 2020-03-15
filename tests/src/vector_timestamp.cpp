#include <cassert>
#include <cstdint>
#include <cstring>

#include <array>

#include <gtest/gtest.h>

#include "hton.hpp"
#include "vector_timestamp.hpp"

TEST(vector_timestamp_test, construction) {
  const vc::vector_timestamp vstamp(vc::actor_id{1});

  const auto json_string = vstamp.to_json();
  const QString expected{"{\"actor1\":0}"};

  EXPECT_EQ(expected, json_string);
}

TEST(vector_timestamp_test, deserialization_too_few_bytes) {
  const uint32_t dummy = 0;
  const auto exp = vc::vector_timestamp::deserialize_from_binary(&dummy,
                                                                 sizeof(dummy));

  ASSERT_FALSE(exp.has_value());

  EXPECT_EQ(std::string("Too few bytes were supplied."),
            exp.error().message().substr(0, 28));
}

TEST(vector_timestamp_test, deserialization_invalid_pair_count) {
  const uint64_t pair_count = 1;
  const auto exp = vc::vector_timestamp::deserialize_from_binary(
    &pair_count, sizeof(pair_count));

  ASSERT_FALSE(exp.has_value());

  EXPECT_EQ(std::string("The pair count given was invalid."),
            exp.error().message().substr(0, 33));
}

TEST(vector_timestamp_test, deserialization) {
  uint64_t pair_count = 3;

  uint64_t actor1_key = 1;
  uint64_t actor1_clock = 1234;

  uint64_t actor2_key = 2;
  uint64_t actor2_clock = 65535;

  uint64_t actor3_key = 3;
  uint64_t actor3_clock = 123456789;

  pair_count = vc::hton(pair_count);
  actor1_key = vc::hton(actor1_key);
  actor1_clock = vc::hton(actor1_clock);
  actor2_key = vc::hton(actor2_key);
  actor2_clock = vc::hton(actor2_clock);
  actor3_key = vc::hton(actor3_key);
  actor3_clock = vc::hton(actor3_clock);

  constexpr auto byte_count = sizeof(pair_count) + sizeof(actor1_key)
                              + sizeof(actor1_clock) + sizeof(actor2_key)
                              + sizeof(actor2_clock) + sizeof(actor3_key)
                              + sizeof(actor3_clock);

  std::array<pl::byte, byte_count> buffer;
  auto* address = buffer.data();

  memcpy(address, &pair_count, sizeof(pair_count));
  address += sizeof(pair_count);

  memcpy(address, &actor1_key, sizeof(actor1_key));
  address += sizeof(actor1_key);

  memcpy(address, &actor1_clock, sizeof(actor1_clock));
  address += sizeof(actor1_clock);

  memcpy(address, &actor2_key, sizeof(actor2_key));
  address += sizeof(actor2_key);

  memcpy(address, &actor2_clock, sizeof(actor2_clock));
  address += sizeof(actor2_clock);

  memcpy(address, &actor3_key, sizeof(actor3_key));
  address += sizeof(actor3_key);

  memcpy(address, &actor3_clock, sizeof(actor3_clock));

  const auto expected_vector_timestamp
    = vc::vector_timestamp::deserialize_from_binary(buffer.data(),
                                                    buffer.size());

  ASSERT_TRUE(expected_vector_timestamp.has_value());

  const auto vector_timestamp = *expected_vector_timestamp;

  const QString expected_json(
    "{\"actor3\":123456789, \"actor1\":1234, \"actor2\":65535}");

  const auto actual_json = vector_timestamp.to_json();

  EXPECT_EQ(expected_json, actual_json);
}

TEST(vector_timestamp_test, tick_not_found) {
  vc::vector_timestamp vstamp(vc::actor_id{1});

  const auto opt = vstamp.tick(vc::actor_id{2});

  EXPECT_FALSE(opt.has_value());
}

TEST(vector_timestamp_test, tick) {
  vc::vector_timestamp vstamp(vc::actor_id{1});

  const auto opt = vstamp.tick(vc::actor_id{1});

  ASSERT_TRUE(opt.has_value());

  const auto clock_value = *opt;

  EXPECT_EQ(1, clock_value);

  const QString expected_json("{\"actor1\":1}");

  EXPECT_EQ(expected_json, vstamp.to_json());
}

TEST(vector_timestamp_test, merge_should_add_new_clock) {
  vc::vector_timestamp vstamp1(vc::actor_id{1});
  const auto opt = vstamp1.tick(vc::actor_id{1});

  ASSERT_TRUE(opt.has_value());

  const vc::vector_timestamp vstamp2(vc::actor_id{2});

  vstamp1.merge(vstamp2);

  const QString expected_json("{\"actor2\":0, \"actor1\":1}");
  const auto actual_json = vstamp1.to_json();

  ASSERT_EQ(expected_json, actual_json);
}

TEST(vector_timestamp_test, merge_should_pick_maximum) {
  vc::vector_timestamp vstamp1(vc::actor_id{3});
  vc::vector_timestamp vstamp2(vc::actor_id{3});

  for (int i = 0; i < 5; ++i) {
    const auto opt = vstamp1.tick(vc::actor_id{3});
    ASSERT_TRUE(opt.has_value());
  }

  for (int i = 0; i < 6; ++i) {
    const auto opt = vstamp2.tick(vc::actor_id{3});
    ASSERT_TRUE(opt.has_value());
  }

  vstamp1.merge(vstamp2);

  const QString expected_json("{\"actor3\":6}");

  EXPECT_EQ(expected_json, vstamp1.to_json());
}

TEST(vector_timestamp_test, to_json) {
  const vc::vector_timestamp vstamp(vc::actor_id{0});

  const QString expected_json("{\"actor0\":0}");

  EXPECT_EQ(expected_json, vstamp.to_json());
}

namespace {
vc::vector_timestamp create(std::unordered_map<vc::actor_id, uint64_t> map) {
  auto it = map.begin();
  const auto [first_aid, first_clock] = *it;

  vc::vector_timestamp vstamp(first_aid);

  for (uint64_t i = 0; i < first_clock; ++i) {
    const auto opt = vstamp.tick(first_aid);
    assert(opt.has_value() && "opt had no value.");
    (void) opt;
  }

  map.erase(it);

  for (const auto [aid, clock] : map) {
    vc::vector_timestamp cur_vstamp(aid);

    for (uint64_t i = 0; i < clock; ++i) {
      const auto opt = cur_vstamp.tick(aid);
      assert(opt.has_value() && "opt had no value!");
      (void) opt;
    }

    vstamp.merge(cur_vstamp);
  }

  return vstamp;
}
} // namespace

TEST(vector_timestamp_test, serialization) {
  const std::unordered_map<vc::actor_id, uint64_t> map{
    {vc::actor_id{0}, 5}, {vc::actor_id{1}, 8}, {vc::actor_id{2}, 20}};

  const auto vstamp = create(map);

  const auto buffer = vstamp.serialize_to_binary();

  const size_t expected_byte_count = 56;

  ASSERT_EQ(expected_byte_count, buffer.size());

  // roundtrip
  const auto exp_stamp = vc::vector_timestamp::deserialize_from_binary(
    buffer.data(), buffer.size());

  ASSERT_TRUE(exp_stamp.has_value());

  const auto stamp = *exp_stamp;

  EXPECT_EQ(vstamp, stamp);
}
