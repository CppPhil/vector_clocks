#pragma once
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
class logger {
public:
  explicit logger(std::ostream& sink);

  template <class FormatString, class... Ts>
  logger& log(const vector_timestamp& vstamp, log_level logger_level,
              actor_id aid, const char* function, const char* file,
              const char* line, FormatString&& format_string, Ts&&... xs) {
    std::lock_guard<std::mutex> lock_guard(mu_);
    (void) lock_guard;

    (*sink_) << fmt::format("{} {} {} {} {}:{} ", vstamp, logger_level, aid,
                            function, file, line);

    (*sink_) << fmt::format(std::forward<FormatString>(format_string),
                            std::forward<Ts>(xs)...)
             << '\n';

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
