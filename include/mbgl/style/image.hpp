#pragma once

#include <mbgl/util/image.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <utility>
#include <vector>

namespace mbgl {
namespace style {

using ImageStretch = std::pair<float, float>;
using ImageStretches = std::vector<ImageStretch>;

class ImageContent {
public:
    float left;
    float top;
    float right;
    float bottom;

    bool operator==(const ImageContent& rhs) const {
        return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
    }
};

class Image {
public:
    Image(std::string id,
          PremultipliedImage&&,
          float pixelRatio,
          bool sdf,
          ImageStretches stretchX = {},
          ImageStretches stretchY = {},
          const optional<ImageContent>& content = nullopt);
    Image(std::string id,
          PremultipliedImage&& image,
          float pixelRatio,
          ImageStretches stretchX = {},
          ImageStretches stretchY = {},
          const optional<ImageContent>& content = nullopt)
        : Image(std::move(id), std::move(image), pixelRatio, false, std::move(stretchX), std::move(stretchY), content) {
    }
    Image(const Image&);

    std::string getID() const;

    const PremultipliedImage& getImage() const;

    // Pixel ratio of the sprite image.
    float getPixelRatio() const;

    // Whether this image should be interpreted as a signed distance field icon.
    bool isSdf() const;

    // Stretch areas of this image.
    const ImageStretches& getStretchX() const;
    const ImageStretches& getStretchY() const;

    // The space where text can be fit into this image.
    const optional<ImageContent>& getContent() const;

    class Impl;
    Immutable<Impl> baseImpl;
    explicit Image(Immutable<Impl> baseImpl_) : baseImpl(std::move(baseImpl_)) {}
};

} // namespace style
} // namespace mbgl
