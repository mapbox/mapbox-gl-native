#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>

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

    inline std::string getAccessToken() const {
        Lock lock(mtx);
        return accessToken;
    }
    inline void setAccessToken(const std::string &token) {
        Lock lock(mtx);
        accessToken = token;
    }

    // Adds the class if it's not yet set. Returns true when it added the class, and false when it
    // was already present.
    bool addClass(const std::string& klass);

    // Removes the class if it's present. Returns true when it remvoed the class, and false when it
    // was not present.
    bool removeClass(const std::string& klass);

    // Returns true when class is present in the list of currently set classes.
    bool hasClass(const std::string& klass) const;

    // Changes the list of currently set classes to the new list.
    void setClasses(const std::vector<std::string>& klasses);

    // Returns a list of all currently set classes.
    std::vector<std::string> getClasses() const;


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
    std::string accessToken;
    std::vector<std::string> classes;
    std::atomic<uint8_t> debug { false };
    std::atomic<std::chrono::steady_clock::time_point::duration> animationTime;
    std::atomic<std::chrono::steady_clock::duration> defaultTransitionDuration;
};

}

#endif
