#include <llmr/renderer/frame_history.hpp>

using namespace llmr;

// Record frame history that will be used to calculate fading params
void FrameHistory::record(float zoom) {
    // first frame ever
    if (!history.size()) {
        history.emplace_back(FrameSnapshot{0.0f, zoom});
        history.emplace_back(FrameSnapshot{0.0f, zoom});
    }

    if (history.size() == 2 || history.back().z != zoom) {
        history.emplace_back(FrameSnapshot{static_cast<float>(platform::time() * 1000), zoom});
    }
}
