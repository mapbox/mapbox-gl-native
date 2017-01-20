#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>

#include <string>
#include <memory>
#include <cstdint>

namespace mbgl {

class SpriteImage : private util::noncopyable {
public:
    SpriteImage(PremultipliedImage&&, float pixelRatio, bool sdf = false);

    PremultipliedImage image;

    // Pixel ratio of the sprite image.
    const float pixelRatio;

    // Whether this image should be interpreted as a signed distance field icon.
    const bool sdf;

    float getWidth() const { return image.size.width / pixelRatio; }
    float getHeight() const { return image.size.height / pixelRatio; }
};

} // namespace mbgl
