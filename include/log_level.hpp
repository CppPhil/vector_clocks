#pragma once
#include <iosfwd>

namespace vc {
enum class log_level { trace, debug, info, warning, error, critical };

std::ostream& operator<<(std::ostream& os, log_level ll);
} // namespace vc
