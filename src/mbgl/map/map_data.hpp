#ifndef MBGL_MAP_MAP_DATA
#define MBGL_MAP_MAP_DATA

#include <mbgl/util/chrono.hpp>

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
