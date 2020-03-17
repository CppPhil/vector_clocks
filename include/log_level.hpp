#pragma once
#include <iosfwd>

namespace vc {
/**
 * The log levels.
 *
 * The log levels.
 * Right now only info shall be used.
 */
enum class log_level { trace, debug, info, warning, error, critical };

/**
 * Prints a log_level enumerator to an ostream.
 * @param os The ostream to print to.
 * @param ll The log_level to print.
 * @return A reference to `os`.
 */
std::ostream& operator<<(std::ostream& os, log_level ll);
} // namespace vc
