#ifndef MBGL_SPRITE_IMAGE
#define MBGL_SPRITE_IMAGE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>

#include <string>
#include <memory>
#include <cstdint>

namespace mbgl {

class SpriteImage : private util::noncopyable {
public:
    SpriteImage(PremultipliedImage&&, float pixelRatio, bool sdf = false, vec2<float> offset = {0.0, 0.0});

    PremultipliedImage image;

    // Pixel ratio of the sprite image.
    const float pixelRatio;

    // Whether this image should be interpreted as a signed distance field icon.
    const bool sdf;

    float getWidth() const { return image.width / pixelRatio; }
    float getHeight() const { return image.height / pixelRatio; }
    
    // position offset
    const vec2<float> offset;
};

} // namespace mbgl

#endif
