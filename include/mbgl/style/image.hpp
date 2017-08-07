#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/immutable.hpp>

#include <string>

namespace mbgl {
namespace style {

class Image {
public:
    Image(std::string id, PremultipliedImage&&, float pixelRatio, bool sdf = false);
    Image(const Image&);

    std::string getID() const;

    const PremultipliedImage& getImage() const;

    // Pixel ratio of the sprite image.
    float getPixelRatio() const;

    // Whether this image should be interpreted as a signed distance field icon.
    bool isSdf() const;

    class Impl;
    Immutable<Impl> baseImpl;
};

} // namespace style
} // namespace mbgl
