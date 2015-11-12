#ifndef MBGL_ANNOTATIONS_SPRITE_IMAGE
#define MBGL_ANNOTATIONS_SPRITE_IMAGE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>

#include <string>
#include <memory>
#include <cstdint>

namespace mbgl {

class SpriteImage : private util::noncopyable {
public:
    SpriteImage(
        float width, float height, float pixelRatio, std::string&& data, bool sdf = false);

    // Logical dimensions of the sprite image.
    const float width;
    const float height;

    // Pixel ratio of the sprite image.
    const float pixelRatio;

    // Physical dimensions of the sprite image.
    const uint16_t pixelWidth;
    const uint16_t pixelHeight;

    // A string of an RGBA8 representation of the sprite. It must have exactly
    // (width * ratio) * (height * ratio) * 4 (RGBA) bytes. The scan lines may
    // not have gaps between them (i.e. stride == 0).
    const std::string data;

    // Whether this image should be interpreted as a signed distance field icon.
    const bool sdf;
};

}

#endif
