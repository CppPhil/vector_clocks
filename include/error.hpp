#pragma once
#include <string>

#include <tl/expected.hpp>

#include "source_location.hpp"

namespace vc {
/**
 * Error type used to indicate failures employing tl::expected.
 */
class error {
public:
  /**
   * Creates a new error object.
   * @param error_message The error message string to use.
   */
  explicit error(std::string error_message);

  /**
   * Read accessor for this error's error message string.
   * @return A reference to the error message string.
   */
  const std::string& message() const;

  /**
   * Throws an exception of type `Exception` using this
   * error's error message string as the error message.
   * @tparam Exception The exception type to use.
   */
  template <class Exception>
  [[noreturn]] void throw_as() const {
    throw Exception(message());
  }

private:
  std::string error_message_;
};
} // namespace vc

/**
 * @def VC_MAKE_ERROR
 * Macro to create a vc::error object that includes the current source location
 * in its error message.
 *
 * Expects the error message to use as an argument.
 */
#define VC_MAKE_ERROR(message)                                                 \
  ::vc::error(::std::string(message)                                           \
              + "\nerror occurred at:\n" VC_SOURCE_LOCATION)

/**
 * @def VC_UNEXPECTED
 * Macro to create a tl::expected with an error.
 *
 * Expects the error message to use as an argument.
 */
#define VC_UNEXPECTED(message) ::tl::make_unexpected(VC_MAKE_ERROR(message))
