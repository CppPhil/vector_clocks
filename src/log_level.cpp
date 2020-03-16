#include <ostream>

#include <QtGlobal>

#include "log_level.hpp"

namespace vc {
std::ostream& operator<<(std::ostream& os, log_level ll) {
  switch (ll) {
    case log_level::trace:
      os << "TRACE";
      break;
    case log_level::debug:
      os << "DEBUG";
      break;
    case log_level::info:
      os << "INFO";
      break;
    case log_level::warning:
      os << "WARNING";
      break;
    case log_level::error:
      os << "ERROR";
      break;
    case log_level::critical:
      os << "CRITICAL";
      break;
    default:
      Q_UNREACHABLE();
      break;
  }

  return os;
}
} // namespace vc
