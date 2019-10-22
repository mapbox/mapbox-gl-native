#include <assert.h>
#include <chrono>
#include <mbgl/util/monotonic_timer.hpp>

namespace mbgl {
namespace util {

// Prefer high resolution timer if it is monotonic
template <typename T, std::enable_if_t<std::chrono::high_resolution_clock::is_steady, T>* = nullptr>
static T sample() {
    return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now().time_since_epoch());
}

template <typename T, std::enable_if_t<!std::chrono::high_resolution_clock::is_steady, T>* = nullptr>
static T sample() {
    return std::chrono::duration_cast<T>(std::chrono::steady_clock::now().time_since_epoch());
}

std::chrono::duration<double> MonotonicTimer::now() {
    return sample<std::chrono::duration<double>>();
}

} // namespace util
} // namespace mbgl