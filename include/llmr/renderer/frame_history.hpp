#ifndef LLMR_RENDERER_FRAME_HISTORY
#define LLMR_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>

#include <llmr/platform/platform.hpp>
#include <llmr/util/time.hpp>

namespace llmr {

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
