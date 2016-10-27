#pragma once

#include <array>

#include <mbgl/platform/platform.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class FrameHistory {
public:
    FrameHistory();
    void record(const TimePoint&, float zoom, const Duration&);

    bool needsAnimation(const Duration&) const;
    void bind(gl::Context&, uint32_t);
    void upload(gl::Context&, uint32_t);

private:
    std::array<TimePoint, 256> changeTimes;
    std::array<uint8_t, 256> changeOpacities;
    const AlphaImage opacities{ { 256, 1 } };

    int16_t previousZoomIndex = 0;
    TimePoint previousTime = TimePoint::min();
    TimePoint time = TimePoint::min();
    bool firstFrame = true;
    bool dirty = true;

    mbgl::optional<gl::Texture> texture;
};

} // namespace mbgl
