#pragma once
#include <cstdint>

#include <QString>

#include <pl/total_order.hpp>

namespace vc {
/**
 * Type used for the identifiers of actors.
 */
class actor_id {
public:
  /**
   * Creates an actor_id from an integer.
   * @param aid The integer to use for the actor_id.
   * @warning Each actor_id shall be unique.
   */
  constexpr explicit actor_id(uint64_t aid) noexcept : aid_(aid) {
  }

  /**
   * Read accessor for the actor_id's underlying integer value.
   * @return The underlying integer value of this actor_id.
   */
  [[nodiscard]] constexpr uint64_t value() const noexcept {
    return aid_;
  }

  /**
   * Returns a QString representation of this actor_id.
   * @return A QString representation of this actor_id.
   */
  [[nodiscard]] QString to_string() const;

  /**
   * Equality compares two actor_ids.
   * @param lhs The first actor_id.
   * @param rhs The second actor_id.
   * @return true if the two actor_ids are considered equal; false otherwise.
   */
  [[nodiscard]] constexpr friend bool operator==(actor_id lhs,
                                                 actor_id rhs) noexcept {
    return lhs.value() == rhs.value();
  }

  /**
   * Implements less than comparison of actor_ids.
   * @param lhs The first actor_id.
   * @param rhs The first actor_id.
   * @return true if `lhs` is considered to be less than `rhs`; false otherwise.
   */
  [[nodiscard]] constexpr friend bool operator<(actor_id lhs,
                                                actor_id rhs) noexcept {
    return lhs.value() < rhs.value();
  }

private:
  uint64_t aid_;
};

PL_TOTAL_ORDER_CONSTEXPR(actor_id)

} // namespace vc

namespace std {
template <>
struct hash<::vc::actor_id> {
  size_t operator()(const ::vc::actor_id& aid) const {
    return ::std::hash<uint64_t>()(aid.value());
  }
};
} // namespace std
