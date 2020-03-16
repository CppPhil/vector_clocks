#include "logger.hpp"

namespace vc {
logger::logger(std::ostream& sink) : mu_(), sink_(&sink) {
  std::lock_guard<std::mutex> lock_guard(mu_);
  (void) lock_guard;

  constexpr char regex[]
    = R"~((?<clock>{.+}) (?<level>INFO) (?<host>\w+\d+) (?<function>.+) (?<file>.+):(?<line>\d+) (?<event>.+))~";

  (*sink_) << regex << "\n\n" << std::flush;

  printf("Wrote \"%s\" to log.\n", regex);
}
} // namespace vc
