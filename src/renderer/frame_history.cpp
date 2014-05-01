#include <llmr/renderer/frame_history.hpp>

using namespace llmr;

// Record frame history that will be used to calculate fading params
void FrameHistory::record(time now, float zoom) {
    // first frame ever
    if (!history.size()) {
        history.emplace_back(FrameSnapshot{0, zoom});
        history.emplace_back(FrameSnapshot{0, zoom});
    }

    if (history.size() > 0 || history.back().z != zoom) {
        history.emplace_back(FrameSnapshot{now, zoom});
    }
}

bool FrameHistory::needsAnimation(const time duration) const {
    if (!history.size()) {
        return false;
    }

    // If we have a value that is older than duration and whose z value is the
    // same as the most current z value, and if all values inbetween have the
    // same z value, we don't need animation, otherwise we probably do.
    const FrameSnapshot &pivot = history.back();

    int i = -1;
    while ((int)history.size() > i + 1 && history[i + 1].timestamp + duration < pivot.timestamp) {
        i++;
    }

    if (i < 0) {
        // There is no frame that is older than the duration time, so we need to
        // check all frames.
        i = 0;
    }

    // Make sure that all subsequent snapshots have the same zoom as the last
    // pivot element.
    for (; (int)history.size() > i; i++) {
        if (history[i].z != pivot.z) {
            return true;
        }
    }

    return false;
}