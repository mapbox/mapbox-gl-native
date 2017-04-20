#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {

ImageSource::Impl::Impl(std::string id_, std::vector<LatLng> coords_)
    : Source::Impl(SourceType::Image, std::move(id_)),
    coords(std::move(coords_)) {
}

ImageSource::Impl::Impl(const Impl& other, std::vector<LatLng> coords_)
    : Source::Impl(other),
    coords(std::move(coords_)),
    image(other.image.clone()) {
}

ImageSource::Impl::Impl(const Impl& rhs, UnassociatedImage image_)
    : Source::Impl(rhs),
    coords(rhs.coords),
    image(std::move(image_)) {
}
ImageSource::Impl::~Impl() = default;

void ImageSource::Impl::setImage(UnassociatedImage image_) {
    image = std::move(image_);
}

const UnassociatedImage& ImageSource::Impl::getImage() const {
    return image;
}

std::vector<LatLng> ImageSource::Impl::getCoordinates() const {
    return coords;
}

optional<std::string> ImageSource::Impl::getAttribution() const {
    return {};
}

} // namespace style
} // namespace mbgl
