#pragma once
#include <type_traits>

#include <QtGlobal>

#include <pl/bswap.hpp>
#include <pl/meta/remove_cvref.hpp>

namespace vc {
template <class T>
inline T hton(const T& x) {
  static_assert(std::is_integral_v<pl::meta::remove_cvref_t<T>>,
                "T must be an integral type in hton.");

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
  return pl::bswap(x);
#elif Q_BYTE_ORDER == Q_BIG_ENDIAN
  return x;
#else
#  error "The host byte order is not supported."
#endif
}
} // namespace vc
