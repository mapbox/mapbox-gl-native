#pragma once

#include <array>

#include <mbgl/platform/platform.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

namespace gl {
class Config;
} // namespace gl

class FrameHistory {
public:
    FrameHistory();
    void record(const TimePoint&, float zoom, const Duration&);

    bool needsAnimation(const Duration&) const;
    void bind(gl::ObjectStore&, gl::Config&, uint32_t);
    void upload(gl::ObjectStore&, gl::Config&, uint32_t);

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

    mbgl::optional<gl::UniqueTexture> texture;
};

} // namespace mbgl
