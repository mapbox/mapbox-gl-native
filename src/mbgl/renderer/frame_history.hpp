#ifndef MBGL_RENDERER_FRAME_HISTORY
#define MBGL_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

struct FrameSnapshot {
    const TimePoint now;
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
    void record(const TimePoint&, float zoom);

    bool needsAnimation(const Duration&) const;
    FadeProperties getFadeProperties(const TimePoint&, const Duration&);

private:
    std::deque<FrameSnapshot> history;
};

} // namespace mbgl

#endif
