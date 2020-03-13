#pragma once
#include <cstdint>

#include <QString>

#include <pl/total_order.hpp>

namespace vc {
class actor_id {
public:
  constexpr explicit actor_id(uint64_t aid) noexcept : aid_(aid) {
  }

  [[nodiscard]] constexpr uint64_t value() const noexcept {
    return aid_;
  }

  [[nodiscard]] QString to_string() const;

  [[nodiscard]] constexpr friend bool operator==(actor_id lhs,
                                                 actor_id rhs) noexcept {
    return lhs.value() == rhs.value();
  }

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
