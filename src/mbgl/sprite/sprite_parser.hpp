#ifndef MBGL_SPRITE_PARSER
#define MBGL_SPRITE_PARSER

#include <mapbox/variant.hpp>

#include <mbgl/util/image.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>

#include <string>
#include <memory>
#include <map>

namespace mbgl {

class SpriteImage;

using SpriteImagePtr = std::shared_ptr<const SpriteImage>;

// Extracts an individual image from a spritesheet from the given location.
SpriteImagePtr createSpriteImage(const PremultipliedImage&,
                                 uint16_t srcX,
                                 uint16_t srcY,
                                 uint16_t srcWidth,
                                 uint16_t srcHeight,
                                 double ratio,
                                 bool sdf);

using Sprites = std::map<std::string, SpriteImagePtr>;


using SpriteParseResult = mapbox::util::variant<
    Sprites,      // success
    std::string>; // error

// Parses an image and an associated JSON file and returns the sprite objects.
SpriteParseResult parseSprite(const std::string& image, const std::string& json);

} // namespace mbgl

#endif
