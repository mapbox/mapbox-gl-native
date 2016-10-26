#include <mbgl/sprite/sprite_image.hpp>

#include <mbgl/util/exception.hpp>

#include <cmath>

namespace mbgl {

SpriteImage::SpriteImage(PremultipliedImage&& image_,
                         const float pixelRatio_,
                         bool sdf_)
    : image(std::move(image_)),
      pixelRatio(pixelRatio_),
      sdf(sdf_) {

    if (!image.valid()) {
        throw util::SpriteImageException("Sprite image dimensions may not be zero");
    } else if (pixelRatio <= 0) {
        throw util::SpriteImageException("Sprite pixelRatio may not be <= 0");
    }
}

} // namespace mbgl
