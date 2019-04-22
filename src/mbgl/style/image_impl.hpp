#pragma once

#include <mbgl/style/image.hpp>

#include <string>
#include <unordered_map>
#include <set>

namespace mbgl {
namespace style {

class Image::Impl {
public:
    Impl(std::string id, PremultipliedImage&&, float pixelRatio, bool sdf = false);

    const std::string id;

    PremultipliedImage image;

    // Pixel ratio of the sprite image.
    const float pixelRatio;

    // Whether this image should be interpreted as a signed distance field icon.
    const bool sdf;
};

} // namespace style

enum class ImageType : bool {
    Icon,
    Pattern
};

using ImageMap = std::unordered_map<std::string, Immutable<style::Image::Impl>>;
using ImageDependencies = std::unordered_map<std::string, ImageType>;
using ImageRequestPair = std::pair<ImageDependencies, uint64_t>;
using ImageVersionMap = std::unordered_map<std::string, uint32_t>;

} // namespace mbgl
