#pragma once

#include <mbgl/style/image.hpp>

namespace mbgl {
namespace style {

class Image::Impl {
public:
    Impl(PremultipliedImage&&, float pixelRatio, bool sdf = false);

    PremultipliedImage image;

    // Pixel ratio of the sprite image.
    const float pixelRatio;

    // Whether this image should be interpreted as a signed distance field icon.
    const bool sdf;

    float getWidth() const { return image.size.width / pixelRatio; }
    float getHeight() const { return image.size.height / pixelRatio; }
};

} // namespace style
} // namespace mbgl
