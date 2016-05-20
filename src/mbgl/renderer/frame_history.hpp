#ifndef MBGL_RENDERER_FRAME_HISTORY
#define MBGL_RENDERER_FRAME_HISTORY

#include <array>

#include <mbgl/platform/platform.hpp>
#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class FrameHistory {
public:
    FrameHistory();
    void record(const TimePoint&, float zoom, const Duration&);

    bool needsAnimation(const Duration&) const;
    void bind(gl::GLObjectStore&);
    void upload(gl::GLObjectStore&);

private:
    const int width = 256;
    const int height = 1;

    std::array<TimePoint, 256> changeTimes;
    std::array<uint8_t, 256> changeOpacities;
    std::array<uint8_t, 256> opacities;

    int16_t previousZoomIndex = 0;
    TimePoint previousTime = TimePoint::min();
    TimePoint time = TimePoint::min();
    bool firstFrame = true;
    bool changed = true;

    gl::TextureHolder texture;
};

} // namespace mbgl

#endif
