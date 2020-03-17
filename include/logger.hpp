#pragma once
#include <cstdio>

#include <mutex>
#include <ostream>
#include <utility>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <pl/current_function.hpp>
#include <pl/source_line.hpp>

#include "log_level.hpp"
#include "vector_timestamp.hpp"

namespace vc {
/**
 * Type for the logger.
 */
class logger {
public:
  /**
   * Creates a logger using the given ostream.
   * @param sink The ostream to write log entries to.
   *
   * Writes the ShiViz-compatible regex to `sink`.
   */
  explicit logger(std::ostream& sink);

  /**
   * Writes a log entry to the log sink.
   * @tparam FormatString The type of the fmtlib compatible format string to
   *                      use.
   * @tparam Ts Template type parameter pack for the arguments to fmt::format.
   * @param vstamp The curent vector timestamp of the entity creating this log
   *               entry.
   * @param logger_level The log level to use.
   * @param aid The actor_id of the entity creating this log entry.
   * @param function The executing function creating this log entry.
   * @param file The current file of the calling context.
   * @param line The line of where the log entry is created.
   * @param format_string The format string to pass to fmt::format.
   * @param xs The arguments to pass to fmt::format.
   * @return A reference to this object.
   */
  template <class FormatString, class... Ts>
  logger& log(const vector_timestamp& vstamp, log_level logger_level,
              actor_id aid, const char* function, const char* file,
              const char* line, FormatString&& format_string, Ts&&... xs) {
    // log_level shall be info, as that's the only level that the regex will
    // accept right now.
    if (logger_level != log_level::info) {
      fprintf(stderr, "Log level isn't info!\n");
      return *this;
    }

    std::lock_guard<std::mutex> lock_guard(mu_);
    (void) lock_guard;

    const auto str
      = fmt::format("{} {} {} {} {}:{} \"{}\"", vstamp, logger_level, aid,
                    function, file, line,
                    fmt::format(std::forward<FormatString>(format_string),
                                std::forward<Ts>(xs)...));

    (*sink_) << str << std::endl;

    std::printf("Wrote \"%s\" to log.\n", str.data());

    return *this;
  }

private:
  std::mutex mu_;
  std::ostream* sink_;
};
} // namespace vc

#define VC_LOG_IMPL(logger, vstamp, log_level, aid, fmt_str, ...)              \
  (logger).log(vstamp, log_level, aid, PL_CURRENT_FUNCTION, __FILE__,          \
               PL_SOURCE_LINE, fmt_str, __VA_ARGS__)

#define VC_LOG_TRACE(logger, vstamp, aid, fmt_str, ...)                        \
  VC_LOG_IMPL(logger, vstamp, ::vc::log_level::trace, aid, fmt_str, __VA_ARGS__)

#define VC_LOG_DEBUG(logger, vstamp, aid, fmt_str, ...)                        \
  VC_LOG_IMPL(logger, vstamp, ::vc::log_level::debug, aid, fmt_str, __VA_ARGS__)

#define VC_LOG_INFO(logger, vstamp, aid, fmt_str, ...)                         \
  VC_LOG_IMPL(logger, vstamp, ::vc::log_level::info, aid, fmt_str, __VA_ARGS__)

#define VC_LOG_WARNING(logger, vstamp, aid, fmt_str, ...)                      \
  VC_LOG_IMPL(logger, vstamp, ::vc::log_level::warning, aid, fmt_str,          \
              __VA_ARGS__)

#define VC_LOG_ERROR(logger, vstamp, aid, fmt_str, ...)                        \
  VC_LOG_IMPL(logger, vstamp, ::vc::log_level::error, aid, fmt_str, __VA_ARGS__)

#define VC_LOG_CRITICAL(logger, vstamp, aid, fmt_str, ...)                     \
  VC_LOG_IMPL(logger, vstamp, ::vc::log_level::critical, aid, fmt_str,         \
              __VA_ARGS__)
