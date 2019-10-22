#pragma once

#include <chrono>
#include <functional>
#include <mbgl/util/noncopyable.hpp>
#include <memory>

namespace mbgl {
namespace util {

class MonotonicTimer {
public:
    static std::chrono::duration<double> now();

    template <typename F, typename... Args>
    inline static std::chrono::duration<double> duration(F&& func, Args&&... args) {
        auto start = now();
        func(std::forward<Args>(args)...);
        return now() - start;
    }
};

} // namespace util
} // namespace mbgl