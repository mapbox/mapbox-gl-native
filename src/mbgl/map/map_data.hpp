#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <string>
#include <mutex>
#include <atomic>
#include <chrono>

namespace mbgl {

struct StyleInfo {
    std::string url;
    std::string base;
    std::string json;
};

class MapData {
    using Lock = std::lock_guard<std::mutex>;

public:
    inline MapData() {
        setAnimationTime(std::chrono::steady_clock::time_point::min());
        setDefaultTransitionDuration(std::chrono::steady_clock::duration::zero());
    }

    inline StyleInfo getStyleInfo() const {
        Lock lock(mtx);
        return styleInfo;
    }
    inline void setStyleInfo(StyleInfo&& info) {
        Lock lock(mtx);
        styleInfo = info;
    }

    inline bool getDebug() const {
        return debug;
    }
    inline bool toggleDebug() {
        return debug ^= 1u;
    }
    inline void setDebug(bool value) {
        debug = value;
    }

    inline std::chrono::steady_clock::time_point getAnimationTime() const {
        // We're casting the time_point to and from a duration because libstdc++
        // has a bug that doesn't allow time_points to be atomic.
        return std::chrono::steady_clock::time_point(animationTime);
    }
    inline void setAnimationTime(std::chrono::steady_clock::time_point timePoint) {
        animationTime = timePoint.time_since_epoch();
    };

    inline std::chrono::steady_clock::duration getDefaultTransitionDuration() const {
        return defaultTransitionDuration;
    }
    inline void setDefaultTransitionDuration(std::chrono::steady_clock::duration duration) {
        defaultTransitionDuration = duration;
    };

private:
    mutable std::mutex mtx;

    StyleInfo styleInfo;
    std::atomic<uint8_t> debug { false };
    std::atomic<std::chrono::steady_clock::time_point::duration> animationTime;
    std::atomic<std::chrono::steady_clock::duration> defaultTransitionDuration;
};

}

#endif
