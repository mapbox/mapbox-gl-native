#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/gl/gl_config.hpp>

namespace mbgl {

FrameHistory::FrameHistory() {
    changeOpacities.fill(0);
    opacities.fill(0);
}

void FrameHistory::record(const TimePoint& now, float zoom, const Duration& duration) {

    int16_t zoomIndex = std::floor(zoom * 10.0);

    if (firstFrame) {
        changeTimes.fill(now);

        for (int16_t z = 0; z <= zoomIndex; z++) {
            opacities[z] = 255u;
        }
        firstFrame = false;
    }

    if (zoomIndex < previousZoomIndex) {
        for (int16_t z = zoomIndex + 1; z <= previousZoomIndex; z++) {
            changeTimes[z] = now;
            changeOpacities[z] = opacities[z];
        }
    } else {
        for (int16_t z = zoomIndex; z > previousZoomIndex; z--) {
            changeTimes[z] = now;
            changeOpacities[z] = opacities[z];
        }
    }

    for (int16_t z = 0; z <= 255; z++) {
        std::chrono::duration<float> timeDiff = now - changeTimes[z];
        int32_t opacityChange = (duration == Milliseconds(0) ? 1 : (timeDiff / duration)) * 255;
        if (z <= zoomIndex) {
            opacities[z] = util::min(255, changeOpacities[z] + opacityChange);
        } else {
            opacities[z] = util::max(0, changeOpacities[z] - opacityChange);
        }
    }

    changed = true;

    if (zoomIndex != previousZoomIndex) {
        previousZoomIndex = zoomIndex;
        previousTime = now;
    }

    time = now;
}

bool FrameHistory::needsAnimation(const Duration& duration) const {
    return (time - previousTime) < duration;
}

void FrameHistory::upload(gl::ObjectStore& store, gl::Config& config, uint32_t unit) {

    if (changed) {
        const bool first = !texture;
        bind(store, config, unit);

        if (first) {
            MBGL_CHECK_ERROR(glTexImage2D(
                        GL_TEXTURE_2D, // GLenum target
                        0, // GLint level
                        GL_ALPHA, // GLint internalformat
                        width, // GLsizei width
                        height, // GLsizei height
                        0, // GLint border
                        GL_ALPHA, // GLenum format
                        GL_UNSIGNED_BYTE, // GLenum type
                        opacities.data()
                        ));
        } else {
            MBGL_CHECK_ERROR(glTexSubImage2D(
                        GL_TEXTURE_2D, // GLenum target
                        0, // GLint level
                        0, // GLint xoffset
                        0, // GLint yoffset
                        width, // GLsizei width
                        height, // GLsizei height
                        GL_ALPHA, // GLenum format
                        GL_UNSIGNED_BYTE, // GLenum type
                        opacities.data()
                        ));
        }

        changed = false;

    }
}

void FrameHistory::bind(gl::ObjectStore& store, gl::Config& config, uint32_t unit) {
    if (!texture) {
        texture = store.createTexture();
        config.activeTexture = unit;
        config.texture[unit] = *texture;
#ifndef GL_ES_VERSION_2_0
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    } else if (config.texture[unit] != *texture) {
        config.activeTexture = unit;
        config.texture[unit] = *texture;
    }
}

} // namespace mbgl
