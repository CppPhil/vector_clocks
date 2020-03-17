#pragma once
#include <cstddef>

#include <vector>

#include <pl/byte.hpp>

#include "error.hpp"

namespace vc {
/**
 * Type used for the packets to send.
 */
class packet {
public:
  /**
   * Creates a packet.
   * @param vstamp_data Pointer to the start of the memory region that
   *                    contains the vector timestamp.
   * @param vstamp_byte_count Size of the vector timestamp in bytes.
   * @param payload_data Pointer to the start of the memory region that
   *                     contains the payload.
   * @param payload_byte_count Size of the payload in bytes.
   */
  packet(const void* vstamp_data, size_t vstamp_byte_count,
         const void* payload_data, size_t payload_byte_count);

  /**
   * Deserializes a packet from a piece of memory.
   * @param data Pointer to the start of the memory region.
   * @param byte_count The size of the binary data containing a packet in bytes.
   * @return An expected containing the packet if the deserialization succeeded;
   * otherwise an error.
   */
  static tl::expected<packet, error> deserialize_from_binary(const void* data,
                                                             size_t byte_count);

  /**
   * Read accessor for the vector timestamp buffer.
   * @return A reference to the vector timestamp buffer.
   */
  [[nodiscard]] const std::vector<pl::byte>& vstamp_buffer() const noexcept;

  /**
   * Read accessor for the payload buffer.
   * @return A reference to the payload buffer.
   */
  [[nodiscard]] const std::vector<pl::byte>& payload_buffer() const noexcept;

  /**
   * Serializes this packet to binary data to be sent over the wire.
   * @return The resulting binary buffer.
   */
  [[nodiscard]] std::vector<pl::byte> serialize_to_binary() const;

private:
  std::vector<pl::byte> vstamp_buffer_;
  std::vector<pl::byte> payload_buffer_;
};
} // namespace vc
