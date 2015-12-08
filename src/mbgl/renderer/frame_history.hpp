#ifndef MBGL_RENDERER_FRAME_HISTORY
#define MBGL_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

struct FrameSnapshot final {
    inline explicit FrameSnapshot(TimePoint now_, float z_) : now(now_), z(z_) {}
    const TimePoint now;
    float z;
};

struct FadeProperties final {
    float fadedist;
    float minfadezoom;
    float maxfadezoom;
    float bump;
};

class FrameHistory final {
public:
    // Record frame history that will be used to calculate fading params
    void record(TimePoint now, float zoom);

    bool needsAnimation(const Duration& duration) const;
    FadeProperties getFadeProperties(TimePoint now, const Duration& duration);

public:
    std::deque<FrameSnapshot> history;
};

} // namespace mbgl

#endif
