#pragma once

#include <mbgl/style/image.hpp>

#include <memory>

namespace mbgl {

// Extracts an individual image from a spritesheet from the given location.
std::unique_ptr<style::Image> createStyleImage(const std::string& id,
                                               const PremultipliedImage&,
                                               uint32_t srcX,
                                               uint32_t srcY,
                                               uint32_t srcWidth,
                                               uint32_t srcHeight,
                                               double ratio,
                                               bool sdf,
                                               style::ImageStretches&& stretchX = {},
                                               style::ImageStretches&& stretchY = {},
                                               optional<style::ImageContent> content = nullopt);

// Parses an image and an associated JSON file and returns the sprite objects.
std::vector<std::unique_ptr<style::Image>> parseSprite(const std::string& image, const std::string& json);

} // namespace mbgl
