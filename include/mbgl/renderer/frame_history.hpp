#ifndef MBGL_RENDERER_FRAME_HISTORY
#define MBGL_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/time.hpp>

namespace mbgl {

struct FrameSnapshot {
    explicit inline FrameSnapshot(timestamp t, float z) : t(t), z(z) {}
    float t;
    float z;
};

class FrameHistory {
public:
    // Record frame history that will be used to calculate fading params
    void record(timestamp now, float zoom);

    bool needsAnimation(timestamp duration) const;

public:
    std::deque<FrameSnapshot> history;
};

}

#endif
