#include "actor_id.hpp"

namespace vc {
actor_id::actor_id(uint64_t aid) noexcept : aid_(aid) {
}

[[nodiscard]] uint64_t actor_id::value() const noexcept {
  return aid_;
}

[[nodiscard]] QString actor_id::to_string() const {
  return QString::number(aid_);
}
} // namespace vc
