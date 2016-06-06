#pragma once

#include <atomic>
#include <mutex>

namespace mbgl {
namespace util {

// std::atomic<bool> is implemented lock free which
// is not supported by ARMv5 but the code generated
// seems to be using that and not working at all,
// thus, this naive implementation using mutexes.
#if defined(__ANDROID__) && defined(__ARM_ARCH_5TE__)

template <typename T>
class Atomic {
public:
    Atomic() = default;
    explicit Atomic(const T& data_) : data(data_) {}

    void operator=(const T& other) {
        std::lock_guard<std::mutex> lock(mtx);
        data = other;
    }

    operator bool() const {
        std::lock_guard<std::mutex> lock(mtx);

        return data;
    }

private:
    T data;
    mutable std::mutex mtx;
};

#else

template <typename T>
using Atomic = std::atomic<T>;

#endif

} // namespace util
} // namespace mbgl
