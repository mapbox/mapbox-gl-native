#include <mbgl/renderer/frame_history.hpp>

using namespace mbgl;

// Record frame history that will be used to calculate fading params
void FrameHistory::record(TimePoint now, float zoom) {
    // first frame ever
    if (!history.size()) {
        history.emplace_back(FrameSnapshot{TimePoint::min(), zoom});
        history.emplace_back(FrameSnapshot{TimePoint::min(), zoom});
    }

    if (history.size() > 0 || history.back().z != zoom) {
        history.emplace_back(FrameSnapshot{now, zoom});
    }
}

bool FrameHistory::needsAnimation(const Duration duration) const {
    if (!history.size()) {
        return false;
    }

    // If we have a value that is older than duration and whose z value is the
    // same as the most current z value, and if all values inbetween have the
    // same z value, we don't need animation, otherwise we probably do.
    const FrameSnapshot &pivot = history.back();

    int i = -1;
    while ((int)history.size() > i + 1 && history[i + 1].now + duration < pivot.now) {
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

FadeProperties FrameHistory::getFadeProperties(Duration duration) {
    const TimePoint currentTime = Clock::now();

    // Remove frames until only one is outside the duration, or until there are only three
    while (history.size() > 3 && history[1].now + duration < currentTime) {
        history.pop_front();
    }

    if (history[1].now + duration < currentTime) {
        history[0].z = history[1].z;
    }

    // Find the range of zoom levels we want to fade between
    float startingZ = history.front().z;
    const FrameSnapshot lastFrame = history.back();
    float endingZ = lastFrame.z;
    float lowZ = std::fmin(startingZ, endingZ);
    float highZ = std::fmax(startingZ, endingZ);

    // Calculate the speed of zooming, and how far it would zoom in terms of zoom levels in one
    // duration
    float zoomDiff = endingZ - history[1].z;
    std::chrono::duration<float> timeDiff = lastFrame.now - history[1].now;
    float fadedist = zoomDiff / (timeDiff / duration);

    // At end of a zoom when the zoom stops changing continue pretending to zoom at that speed
    // bump is how much farther it would have been if it had continued zooming at the same rate
    float bump = std::chrono::duration<float>(currentTime - lastFrame.now) / duration * fadedist;

    return FadeProperties {
        fadedist,
        lowZ,
        highZ,
        bump
    };
}
