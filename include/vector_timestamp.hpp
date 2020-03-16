#pragma once
#include <unordered_map>
#include <vector>

#include <tl/expected.hpp>
#include <tl/optional.hpp>

#include <pl/byte.hpp>

#include "actor_id.hpp"
#include "error.hpp"

namespace vc {
/**
 * The vector timestamp type.
 */
class vector_timestamp {
public:
  /**
   * Creates a vector_timestamp object.
   * @param aid The actor_id to use.
   *
   * Creates a vector_timestamp object containing the actor_id given
   * having it be associated with a clock with a count of 0.
   */
  explicit vector_timestamp(actor_id aid);

  /**
   * Deserializes a vector_timestamp from binary data.
   * @param pointer Pointer to the start of the memory region containing
   *                binary vector_timestamp data.
   * @param byte_count The size of the memory pointed to by `pointer` in bytes.
   * @return An expected containing the vector_timestamp on success; otherwise
   *         an error object.
   */
  [[nodiscard]] static tl::expected<vector_timestamp, error>
  deserialize_from_binary(const void* pointer, size_t byte_count);

  /**
   * Increases the logical clock for the actor_id `aid`.
   * @param aid The actor_id to increase the logical clock of.
   * @return An optional containing the new logical clock of `aid`, or
   *         tl::nullopt if there's no actor_id `aid` in this vector_timestamp.
   */
  [[nodiscard]] tl::optional<uint64_t> tick(actor_id aid);

  /**
   * Merges `other` into this vector_timestamp.
   * @param other The other vector_timestamp that shall be merged into this
   * vector_timestamp.
   * @return A reference to this vector_timestamp object.
   */
  vector_timestamp& merge(const vector_timestamp& other);

  /**
   * Serializes this vector_timestamp to JSON.
   * @return A QString containing JSON data.
   */
  [[nodiscard]] QString to_json() const;

  /**
   * Serialize this vector_timestamp to binary.
   * @return The resulting binary buffer.
   */
  [[nodiscard]] std::vector<pl::byte> serialize_to_binary() const;

  /**
   * Implements equality comparison for vector_timestamps.
   * @param lhs The first vector_timestamp.
   * @param rhs The second vector_timestamp.
   * @return true if `lhs` and `rhs` are considered equal; false otherwise.
   */
  friend bool operator==(const vector_timestamp& lhs,
                         const vector_timestamp& rhs);

  /**
   * Implements inequality comparison for vector_timestamps.
   * @param lhs The first vector_timestamp.
   * @param rhs The second vecotr_timestamp.
   * @return true if `lhs` and `rhs` are considered not equal; false otherwise.
   */
  friend bool operator!=(const vector_timestamp& lhs,
                         const vector_timestamp& rhs);

private:
  explicit vector_timestamp(
    std::unordered_map<actor_id, uint64_t>&& map) noexcept;

  std::unordered_map<actor_id, uint64_t> data_;
};
} // namespace vc
