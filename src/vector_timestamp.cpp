#include <cstring>

#include <utility>

#include "hton.hpp"
#include "ntoh.hpp"
#include "vector_timestamp.hpp"

namespace vc {
vector_timestamp::vector_timestamp(actor_id aid) : data_{{aid, 0}} {
}

[[nodiscard]] tl::expected<vector_timestamp, error>
vector_timestamp::deserialize_from_binary(const void* pointer,
                                          size_t byte_count) {
  if (byte_count < sizeof(uint64_t))
    return VC_UNEXPECTED("Too few bytes were supplied.");

  const auto* ptr = static_cast<const pl::byte*>(pointer);

  const auto read = [&ptr] {
    uint64_t buffer;
    memcpy(&buffer, ptr, sizeof(uint64_t));

    ptr += sizeof(uint64_t);

    return ntoh(buffer);
  };

  const auto pair_count = read();

  if ((pair_count * sizeof(uint64_t)) != (byte_count - sizeof(uint64_t)))
    return VC_UNEXPECTED("The pair count given was invalid.");

  std::unordered_map<actor_id, uint64_t> map;

  for (uint64_t i = 0; i < pair_count; ++i) {
    const actor_id aid(read());
    const auto clock = read();

    map.emplace(aid, clock);
  }

  return vector_timestamp(std::move(map));
}

[[nodiscard]] tl::optional<uint64_t> vector_timestamp::tick(actor_id aid) {
  const auto it = data_.find(aid);

  if (it == data_.end()) // If not found
    return tl::nullopt;
  else { // If found
    auto& [ignored_aid, clock] = *it;
    (void) ignored_aid;

    ++clock;

    return clock;
  }
}

vector_timestamp& vector_timestamp::merge(const vector_timestamp& other) {
  for (const auto [aid, their_clock] : other.data_) {
    auto& own_clock = data_[aid];
    own_clock = std::max(own_clock, their_clock);
  }

  return *this;
}

[[nodiscard]] QString vector_timestamp::to_json() const {
  QString buffer;

  buffer += '{';

  for (const auto [aid, clock] : data_)
    buffer += '"' + aid.to_string() + "\":" + QString::number(clock) + ", ";

  // ", " are two characters (not including the null-terminator)
  buffer.remove(buffer.size() - 2, 2);

  buffer += '}';

  return buffer;
}

[[nodiscard]] std::vector<pl::byte>
vector_timestamp::serialize_to_binary() const {
  std::vector<pl::byte> buffer;

  const auto append_bytes = [&buffer](const void* pointer, size_t byte_count) {
    const auto* const first = static_cast<const pl::byte*>(pointer);
    const auto* const last = first + byte_count;
    buffer.insert(buffer.end(), first, last);
  };

  const auto append = [&append_bytes](const auto& x) {
    append_bytes(&x, sizeof(x));
  };

  const auto pair_count = hton(static_cast<uint64_t>(data_.size()));

  append(pair_count);

  for (const auto [aid, clock] : data_) {
    append(hton(aid.value()));
    append(hton(clock));
  }

  return buffer;
}

vector_timestamp::vector_timestamp(
  std::unordered_map<actor_id, uint64_t>&& map) noexcept
  : data_(std::move(map)) {
}
} // namespace vc
