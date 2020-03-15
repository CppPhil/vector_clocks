#pragma once
#include <unordered_map>
#include <vector>

#include <tl/expected.hpp>
#include <tl/optional.hpp>

#include <pl/byte.hpp>

#include "actor_id.hpp"
#include "error.hpp"

namespace vc {
class vector_timestamp {
public:
  explicit vector_timestamp(actor_id aid);

  [[nodiscard]] static tl::expected<vector_timestamp, error>
  deserialize_from_binary(const void* pointer, size_t byte_count);

  [[nodiscard]] tl::optional<uint64_t> tick(actor_id aid);

  vector_timestamp& merge(const vector_timestamp& other);

  [[nodiscard]] QString to_json() const;

  [[nodiscard]] std::vector<pl::byte> serialize_to_binary() const;

  friend bool operator==(const vector_timestamp& lhs,
                         const vector_timestamp& rhs);

  friend bool operator!=(const vector_timestamp& lhs,
                         const vector_timestamp& rhs);

private:
  explicit vector_timestamp(
    std::unordered_map<actor_id, uint64_t>&& map) noexcept;

  std::unordered_map<actor_id, uint64_t> data_;
};
} // namespace vc
