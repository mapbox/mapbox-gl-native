#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

#include <cassert>

namespace mbgl {

FrameHistory::FrameHistory() {
    changeOpacities.fill(0);
    std::fill(opacities.data.get(), opacities.data.get() + opacities.bytes(), 0);
}

void FrameHistory::record(const TimePoint& now, float zoom, const Duration& duration) {

    int16_t zoomIndex = std::floor(zoom * 10.0);

    if (firstFrame) {
        changeTimes.fill(now);

        for (int16_t z = 0; z <= zoomIndex; z++) {
            opacities.data[z] = 255u;
        }
        firstFrame = false;
    }

    if (zoomIndex < previousZoomIndex) {
        for (int16_t z = zoomIndex + 1; z <= previousZoomIndex; z++) {
            changeTimes[z] = now;
            changeOpacities[z] = opacities.data[z];
        }
    } else {
        for (int16_t z = zoomIndex; z > previousZoomIndex; z--) {
            changeTimes[z] = now;
            changeOpacities[z] = opacities.data[z];
        }
    }

    for (int16_t z = 0; z <= 255; z++) {
        std::chrono::duration<float> timeDiff = now - changeTimes[z];
        int32_t opacityChange = (duration == Milliseconds(0) ? 1 : (timeDiff / duration)) * 255;
        if (z <= zoomIndex) {
            opacities.data[z] = util::min(255, changeOpacities[z] + opacityChange);
        } else {
            opacities.data[z] = util::max(0, changeOpacities[z] - opacityChange);
        }
    }

    dirty = true;

    if (zoomIndex != previousZoomIndex) {
        previousZoomIndex = zoomIndex;
        previousTime = now;
    }

    time = now;
}

bool FrameHistory::needsAnimation(const Duration& duration) const {
    return (time - previousTime) < duration;
}

void FrameHistory::upload(gl::Context& context, uint32_t unit) {
    if (!texture) {
        texture = context.createTexture(opacities, unit);
    } else if (dirty) {
        context.updateTexture(*texture, opacities, unit);
    }
    dirty = false;
}

void FrameHistory::bind(gl::Context& context, uint32_t unit) {
    upload(context, unit);
    context.bindTexture(*texture, unit);
}

} // namespace mbgl
