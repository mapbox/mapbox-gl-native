#pragma once

#include <mbgl/util/image.hpp>

#include <memory>

namespace mbgl {
namespace style {

class Image {
public:
    Image(PremultipliedImage&&, float pixelRatio, bool sdf = false);

    PremultipliedImage& getImage() const;

    // Pixel ratio of the sprite image.
    float getPixelRatio() const;

    // Whether this image should be interpreted as a signed distance field icon.
    bool isSdf() const;

    float getWidth() const;
    float getHeight() const;

    class Impl;

private:
    const std::shared_ptr<Impl> impl;
};

} // namespace style
} // namespace mbgl
