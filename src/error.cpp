#include <utility>

#include "error.hpp"

namespace vc {
error::error(std::string error_message)
  : error_message_(std::move(error_message)) {
}

const std::string& error::message() const {
  return error_message_;
}
} // namespace vc
