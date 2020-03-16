#include <ostream>

#include "actor_id.hpp"

namespace vc {
[[nodiscard]] QString actor_id::to_string() const {
  return QStringLiteral("actor%1").arg(aid_);
}

std::ostream& operator<<(std::ostream& os, const actor_id& aid) {
  return os << aid.to_string().toStdString();
}
} // namespace vc
