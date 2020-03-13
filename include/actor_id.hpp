#pragma once
#include <cstdint>

#include <QString>

namespace vc {
class actor_id {
public:
  explicit actor_id(uint64_t aid) noexcept;

  [[nodiscard]] uint64_t value() const noexcept;

  [[nodiscard]] QString to_string() const;

private:
  uint64_t aid_;
};
} // namespace vc
