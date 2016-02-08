#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <mbgl/util/chrono.hpp>

#include <string>
#include <mutex>
#include <atomic>
#include <vector>
#include <cassert>
#include <condition_variable>

#include <mbgl/map/mode.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/exclusive.hpp>

namespace mbgl {

class MapData {
    using Lock = std::lock_guard<std::mutex>;

public:
    inline MapData(MapMode mode_, GLContextMode contextMode_, const float pixelRatio_)
        : mode(mode_)
        , contextMode(contextMode_)
        , pixelRatio(pixelRatio_)
        , annotationManager(pixelRatio)
        , animationTime(Duration::zero())
        , defaultFadeDuration(mode_ == MapMode::Continuous ? Milliseconds(300) : Duration::zero())
        , defaultTransitionDuration(Duration::zero())
        , defaultTransitionDelay(Duration::zero()) {
        assert(pixelRatio > 0);
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


    inline MapDebugOptions getDebug() const {
        return debugOptions;
    }

    inline void cycleDebugOptions() {
        if (debugOptions & MapDebugOptions::Collision)
            debugOptions = MapDebugOptions::NoDebug;
        else if (debugOptions & MapDebugOptions::Timestamps)
            debugOptions = debugOptions | MapDebugOptions::Collision;
        else if (debugOptions & MapDebugOptions::ParseStatus)
            debugOptions = debugOptions | MapDebugOptions::Timestamps;
        else if (debugOptions & MapDebugOptions::TileBorders)
            debugOptions = debugOptions | MapDebugOptions::ParseStatus;
        else
            debugOptions = MapDebugOptions::TileBorders;
    }

    inline void setDebug(MapDebugOptions debugOptions_) {
        debugOptions = debugOptions_;
    }

    inline TimePoint getAnimationTime() const {
        // We're casting the TimePoint to and from a Duration because libstdc++
        // has a bug that doesn't allow TimePoints to be atomic.
        return mode == MapMode::Continuous ? TimePoint(animationTime) : Clock::now();
    }
    inline void setAnimationTime(const TimePoint& timePoint) {
        if (mode == MapMode::Still) {
            return;
        }

        animationTime = timePoint.time_since_epoch();
    };

    inline Duration getDefaultFadeDuration() const {
        return defaultFadeDuration;
    }

    inline void setDefaultFadeDuration(const Duration& duration) {
        if (mode == MapMode::Still) {
            return;
        }

        defaultFadeDuration = duration;
    }

    inline Duration getDefaultTransitionDuration() const {
        return defaultTransitionDuration;
    }

    inline void setDefaultTransitionDuration(const Duration& duration) {
        if (mode == MapMode::Still) {
            return;
        }

        defaultTransitionDuration = duration;
    }

    inline Duration getDefaultTransitionDelay() const {
        return defaultTransitionDelay;
    }

    inline void setDefaultTransitionDelay(const Duration& delay) {
        if (mode == MapMode::Still) {
            return;
        }

        defaultTransitionDelay = delay;
    }

    util::exclusive<AnnotationManager> getAnnotationManager() {
        return util::exclusive<AnnotationManager>(
            &annotationManager,
            std::make_unique<std::lock_guard<std::mutex>>(annotationManagerMutex));
    }

public:
    const MapMode mode;
    const GLContextMode contextMode;
    const float pixelRatio;

private:
    mutable std::mutex annotationManagerMutex;
    AnnotationManager annotationManager;

    mutable std::mutex mtx;

    std::vector<std::string> classes;
    std::atomic<MapDebugOptions> debugOptions { MapDebugOptions::NoDebug };
    std::atomic<Duration> animationTime;
    std::atomic<Duration> defaultFadeDuration;
    std::atomic<Duration> defaultTransitionDuration;
    std::atomic<Duration> defaultTransitionDelay;

// TODO: make private
public:
    bool paused = false;
    std::mutex mutexPause;
    std::condition_variable condPause;
    bool loading = false;
};

} // namespace mbgl

#endif
