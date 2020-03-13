#include "actor_id.hpp"

namespace vc {
[[nodiscard]] QString actor_id::to_string() const {
  return QStringLiteral("actor%1").arg(aid_);
}
} // namespace vc
