#include <mbgl/sprite/sprite_image.hpp>

#include <mbgl/util/exception.hpp>

#include <cmath>

namespace mbgl {

SpriteImage::SpriteImage(const uint16_t pixelWidth_,
                         const uint16_t pixelHeight_,
                         const float pixelRatio_,
                         std::string&& data_,
                         bool sdf_)
    : width(std::ceil(pixelWidth_ / pixelRatio_)),
      height(std::ceil(pixelHeight_ / pixelRatio_)),
      pixelRatio(pixelRatio_),
      pixelWidth(pixelWidth_),
      pixelHeight(pixelHeight_),
      data(std::move(data_)),
      sdf(sdf_) {
    const size_t size = pixelWidth * pixelHeight * 4;
    if (size == 0) {
        throw util::SpriteImageException("Sprite image dimensions may not be zero");
    } else if (size != data.size()) {
        throw util::SpriteImageException("Sprite image pixel count mismatch");
    }
}

} // namespace mbgl
