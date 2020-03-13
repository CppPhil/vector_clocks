#pragma once
#include <string>

#include <tl/expected.hpp>

#include "source_location.hpp"

namespace vc {
class error {
public:
  explicit error(std::string error_message);

  const std::string& message() const;

  template <class Exception>
  [[noreturn]] void throw_as() const {
    throw Exception(message());
  }

private:
  std::string error_message_;
};
} // namespace vc

#define VC_MAKE_ERROR(message)                                                 \
  ::vc::error(::std::string(message)                                           \
              + "\nerror occurred at:\n" VC_SOURCE_LOCATION)

#define VC_UNEXPECTED(message) ::tl::make_unexpected(VC_MAKE_ERROR(message))
