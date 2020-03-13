#include "actor_id.hpp"

namespace vc {
[[nodiscard]] QString actor_id::to_string() const {
  return QString::number(aid_);
}
} // namespace vc
