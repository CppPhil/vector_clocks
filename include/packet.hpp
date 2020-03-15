#pragma once
#include <cstddef>

#include <vector>

#include <pl/byte.hpp>

#include "error.hpp"

namespace vc {
class packet {
public:
  packet(const void* vstamp_data, size_t vstamp_byte_count,
         const void* payload_data, size_t payload_byte_count);

  static tl::expected<packet, error> deserialize_from_binary(const void* data,
                                                             size_t byte_count);

  const std::vector<pl::byte>& vstamp_buffer() const noexcept;

  const std::vector<pl::byte>& payload_buffer() const noexcept;

  std::vector<pl::byte> serialize_to_binary() const;

private:
  std::vector<pl::byte> vstamp_buffer_;
  std::vector<pl::byte> payload_buffer_;
};
} // namespace vc
