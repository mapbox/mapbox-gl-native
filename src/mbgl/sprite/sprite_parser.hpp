#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/geo.hpp>

#include <string>
#include <memory>
#include <map>

namespace mbgl {

namespace style {
class Image;
} // namespace style

// Extracts an individual image from a spritesheet from the given location.
std::unique_ptr<style::Image> createStyleImage(const PremultipliedImage&,
                                 uint32_t srcX,
                                 uint32_t srcY,
                                 uint32_t srcWidth,
                                 uint32_t srcHeight,
                                 double ratio,
                                 bool sdf);

using Images = std::map<std::string, std::unique_ptr<style::Image>>;

// Parses an image and an associated JSON file and returns the sprite objects.
Images parseSprite(const std::string& image, const std::string& json);

} // namespace mbgl
