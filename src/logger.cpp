#include "logger.hpp"

namespace vc {
logger::logger(std::ostream& sink) : sink_(&sink) {
  (*sink_) << "(?<clock>\\S+) (?<level>\\S+) "
              "(?<host>\\S+) (?<function>\\S+) "
              "(?<file>\\S+):(?<line>\\d+) (?<message>.+)";
}
} // namespace vc
