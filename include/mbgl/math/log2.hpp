#pragma once

#include <cmath>
#include <cstdint>
#include <type_traits>

#if defined(__ANDROID__)
#include <android/api-level.h>
#endif

namespace mbgl {
namespace util {

// Computes the log2(x) rounded up to the next integer.
// (== number of bits required to store x)
uint32_t ceil_log2(uint64_t x);

template <typename T>
T log2(T x) {
// log2() is producing wrong results on ARMv5 binaries
// running on ARMv7+ CPUs.
#if defined(__ANDROID__)
    return ::log(x) / M_LN2;
#else
    return ::log2(x);
#endif
}

} // namespace util
} // namespace mbgl

// log2 is not available on Android before API 18.
#if defined(__ANDROID__) && defined(__GNUC__) && \
    defined(__ANDROID_API__) && __ANDROID_API__ < 18

template <typename T>
typename std::enable_if_t<std::is_floating_point<T>::value, T> log2(T x) {
    return ::log(x) / M_LN2;
}

#endif
