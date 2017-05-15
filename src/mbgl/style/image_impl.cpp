#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {
namespace style {

Image::Impl::Impl(std::string id_,
                  PremultipliedImage&& image_,
                  const float pixelRatio_,
                  bool sdf_)
        : id(std::move(id_)),
          image(std::move(image_)),
          pixelRatio(pixelRatio_),
          sdf(sdf_) {

    if (!image.valid()) {
        throw util::SpriteImageException("Sprite image dimensions may not be zero");
    } else if (pixelRatio <= 0) {
        throw util::SpriteImageException("Sprite pixelRatio may not be <= 0");
    }
}

} // namespace style
} // namespace mbgl
