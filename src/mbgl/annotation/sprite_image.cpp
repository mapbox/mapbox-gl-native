#include <mbgl/annotation/sprite_image.hpp>

#include <mbgl/util/exception.hpp>

#include <cmath>

namespace mbgl {

SpriteImage::SpriteImage(const uint16_t width_,
                         const uint16_t height_,
                         const float pixelRatio_,
                         std::string&& data_,
                         bool sdf_)
    : width(width_),
      height(height_),
      pixelRatio(pixelRatio_),
      pixelWidth(std::ceil(width * pixelRatio)),
      pixelHeight(std::ceil(height * pixelRatio)),
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
