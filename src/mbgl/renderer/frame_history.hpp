#ifndef MBGL_RENDERER_FRAME_HISTORY
#define MBGL_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>
#include <chrono>

#include <mbgl/platform/platform.hpp>

namespace mbgl {

struct FrameSnapshot {
    explicit inline FrameSnapshot(std::chrono::steady_clock::time_point now_, float z_) : now(now_), z(z_) {}
    std::chrono::steady_clock::time_point now;
    float z;
};

struct FadeProperties {
    float fadedist;
    float minfadezoom;
    float maxfadezoom;
    float bump;
};

class FrameHistory {
public:
    // Record frame history that will be used to calculate fading params
    void record(std::chrono::steady_clock::time_point now, float zoom);

    bool needsAnimation(std::chrono::steady_clock::duration duration) const;
    FadeProperties getFadeProperties(std::chrono::steady_clock::duration duration);

public:
    std::deque<FrameSnapshot> history;
};

}

#endif
