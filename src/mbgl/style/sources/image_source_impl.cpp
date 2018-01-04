#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {

ImageSource::Impl::Impl(std::string id_, std::array<LatLng, 4> coords_)
    : Source::Impl(SourceType::Image, std::move(id_)),
    coords(std::move(coords_)) {
}

ImageSource::Impl::Impl(const Impl& other, std::array<LatLng, 4> coords_)
    : Source::Impl(other),
    coords(std::move(coords_)),
    image(other.image) {
}

ImageSource::Impl::Impl(const Impl& rhs, PremultipliedImage&& image_)
    : Source::Impl(rhs),
    coords(rhs.coords),
    image(std::make_shared<PremultipliedImage>(std::move(image_))) {
}
ImageSource::Impl::~Impl() = default;

std::shared_ptr<PremultipliedImage> ImageSource::Impl::getImage() const {
    return image;
}

std::array<LatLng, 4> ImageSource::Impl::getCoordinates() const {
    return coords;
}

optional<std::string> ImageSource::Impl::getAttribution() const {
    return {};
}

} // namespace style
} // namespace mbgl
