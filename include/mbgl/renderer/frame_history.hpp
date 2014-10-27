#ifndef MBGL_RENDERER_FRAME_HISTORY
#define MBGL_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/time.hpp>

namespace mbgl {

struct FrameSnapshot {
    explicit inline FrameSnapshot(timestamp t_, float z_) : t(t_), z(z_) {}
    float t;
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
    void record(timestamp now, float zoom);

    bool needsAnimation(timestamp duration) const;
    FadeProperties getFadeProperties(timestamp duration);

public:
    std::deque<FrameSnapshot> history;
};

}

#endif
