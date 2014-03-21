#ifndef LLMR_RENDERER_FRAME_HISTORY
#define LLMR_RENDERER_FRAME_HISTORY

#include <deque>
#include <cassert>
#include <cmath>

#include <llmr/platform/platform.hpp>

namespace llmr {

struct FrameSnapshot {
    explicit inline FrameSnapshot(float time, float z) : time(time), z(z) {}
    float time;
    float z;
};

class FrameHistory {
public:
    // Record frame history that will be used to calculate fading params
    void record(float zoom);

public:
    std::deque<FrameSnapshot> history;
};

}

#endif
