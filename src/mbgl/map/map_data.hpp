#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <mbgl/util/chrono.hpp>

#include <string>
#include <mutex>
#include <atomic>
#include <vector>
#include <cassert>

#include <mbgl/map/environment.hpp>
#include <mbgl/map/transform_state.hpp>

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
        setAnimationTime(TimePoint::min());
        setDefaultTransitionDuration(Duration::zero());
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

    inline TimePoint getAnimationTime() const {
        // We're casting the TimePoint to and from a Duration because libstdc++
        // has a bug that doesn't allow TimePoints to be atomic.
        return TimePoint(animationTime);
    }
    inline void setAnimationTime(TimePoint timePoint) {
        animationTime = timePoint.time_since_epoch();
    };

    inline Duration getDefaultTransitionDuration() const {
        return defaultTransitionDuration;
    }
    inline void setDefaultTransitionDuration(Duration duration) {
        defaultTransitionDuration = duration;
    };

    // Make sure the state is only accessible/modifiable from the Map thread.
    inline const TransformState& getTransformState() const {
        assert(Environment::currentlyOn(ThreadType::Map));
        return transformState;
    }
    inline void setTransformState(const TransformState& state) {
        assert(Environment::currentlyOn(ThreadType::Map));
        transformState = state;
    }

private:
    mutable std::mutex mtx;

    StyleInfo styleInfo;
    std::string accessToken;
    std::vector<std::string> classes;
    TransformState transformState;
    std::atomic<uint8_t> debug { false };
    std::atomic<Duration> animationTime;
    std::atomic<Duration> defaultTransitionDuration;
};

}

#endif
