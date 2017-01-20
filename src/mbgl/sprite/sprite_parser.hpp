#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/geo.hpp>

#include <string>
#include <memory>
#include <map>

namespace mbgl {

class SpriteImage;

using SpriteImagePtr = std::shared_ptr<const SpriteImage>;

// Extracts an individual image from a spritesheet from the given location.
SpriteImagePtr createSpriteImage(const PremultipliedImage&,
                                 uint32_t srcX,
                                 uint32_t srcY,
                                 uint32_t srcWidth,
                                 uint32_t srcHeight,
                                 double ratio,
                                 bool sdf);

using Sprites = std::map<std::string, SpriteImagePtr>;


using SpriteParseResult = variant<
    Sprites,             // success
    std::exception_ptr>; // error

// Parses an image and an associated JSON file and returns the sprite objects.
SpriteParseResult parseSprite(const std::string& image, const std::string& json);

} // namespace mbgl
