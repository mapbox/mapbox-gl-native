#include <mbgl/style/image.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {
namespace style {

Image::Image(std::string id,
             PremultipliedImage &&image,
             const float pixelRatio,
             bool sdf)
    : baseImpl(makeMutable<Impl>(std::move(id), std::move(image), pixelRatio, sdf)) {
}

std::string Image::getID() const {
    return baseImpl->id;
}

Image::Image(const Image&) = default;

const PremultipliedImage& Image::getImage() const {
    return baseImpl->image;
}

bool Image::isSdf() const {
    return baseImpl->sdf;
}

float Image::getPixelRatio() const {
    return baseImpl->pixelRatio;
}

} // namespace style
} // namespace mbgl
