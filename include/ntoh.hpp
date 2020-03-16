#pragma once
#include <type_traits>

#include <QtGlobal>

#include <pl/bswap.hpp>
#include <pl/meta/remove_cvref.hpp>

namespace vc {
/**
 * Converts something in network byte order to host byte order.
 * @tparam T The type of the object to convert to host byte order.
 * @param x The object to convert.
 * @return The result in host byte order.
 * @warning The argument given must be in network byte order!
 */
template <class T>
inline T ntoh(const T& x) {
  static_assert(std::is_integral_v<pl::meta::remove_cvref_t<T>>,
                "T must be an integral type in ntoh.");

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
  return pl::bswap(x);
#elif Q_BYTE_ORDER == Q_BIG_ENDIAN
  return x;
#else
#  error "The host byte order is not supported."
#endif
}
} // namespace vc
