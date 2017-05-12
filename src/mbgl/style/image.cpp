#include <mbgl/style/image.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {
namespace style {

Image::Image(PremultipliedImage &&image,
             const float pixelRatio,
             bool sdf)
        : impl(std::make_shared<Impl>(std::move(image), pixelRatio, sdf)) {
}

PremultipliedImage& Image::getImage() const {
    assert(impl);
    return impl->image;
}

bool Image::isSdf() const {
    return impl->sdf;
}

float Image::getPixelRatio() const {
    return impl->pixelRatio;
}

float Image::getWidth() const {
    return impl->getWidth();
}

float Image::getHeight() const {
    return impl->getHeight();
}

} // namespace style
} // namespace mbgl
