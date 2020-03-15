#include <cstring>

#include "hton.hpp"
#include "ntoh.hpp"
#include "packet.hpp"

namespace vc {
packet::packet(const void* vstamp_data, size_t vstamp_byte_count,
               const void* payload_data, size_t payload_byte_count)
  : vstamp_buffer_(static_cast<const pl::byte*>(vstamp_data),
                   static_cast<const pl::byte*>(vstamp_data)
                     + vstamp_byte_count),
    payload_buffer_(static_cast<const pl::byte*>(payload_data),
                    static_cast<const pl::byte*>(payload_data)
                      + payload_byte_count) {
}

tl::expected<packet, error> packet::deserialize_from_binary(const void* data,
                                                            size_t byte_count) {
  constexpr auto minimum_byte_count = 2U * sizeof(uint64_t);

  if (byte_count < minimum_byte_count)
    return VC_UNEXPECTED("Too few bytes were provided.");

  const auto* p = static_cast<const pl::byte*>(data);

  uint64_t vstamp_size;
  memcpy(&vstamp_size, p, sizeof(vstamp_size));
  p += sizeof(vstamp_size);
  vstamp_size = ntoh(vstamp_size);

  if (vstamp_size == 0)
    return VC_UNEXPECTED("A vector timestamp may not be 0 bytes wide.");

  const std::vector<pl::byte> vstamp_buf(p, p + vstamp_size);
  p += vstamp_size;

  uint64_t payload_size;
  memcpy(&payload_size, p, sizeof(payload_size));
  p += sizeof(payload_size);
  payload_size = ntoh(payload_size);

  if (payload_size == 0)
    return VC_UNEXPECTED("A payload may not be 0 bytes wide.");

  const std::vector<pl::byte> payload_buf(p, p + payload_size);

  return packet(vstamp_buf.data(), vstamp_buf.size(), payload_buf.data(),
                payload_buf.size());
}

const std::vector<pl::byte>& packet::vstamp_buffer() const noexcept {
  return vstamp_buffer_;
}

const std::vector<pl::byte>& packet::payload_buffer() const noexcept {
  return payload_buffer_;
}

std::vector<pl::byte> packet::serialize_to_binary() const {
  std::vector<pl::byte> buffer(sizeof(uint64_t) + vstamp_buffer().size()
                               + sizeof(uint64_t) + payload_buffer().size());

  const auto vstamp_byte_count
    = hton(static_cast<uint64_t>(vstamp_buffer().size()));

  const auto payload_byte_count
    = hton(static_cast<uint64_t>(payload_buffer().size()));

  auto* pointer = buffer.data();

  memcpy(pointer, &vstamp_byte_count, sizeof(vstamp_byte_count));
  pointer += sizeof(vstamp_byte_count);

  memcpy(pointer, vstamp_buffer().data(), vstamp_buffer().size());
  pointer += vstamp_buffer().size();

  memcpy(pointer, &payload_byte_count, sizeof(payload_byte_count));
  pointer += sizeof(payload_byte_count);

  memcpy(pointer, payload_buffer().data(), payload_buffer().size());

  return buffer;
}
} // namespace vc
