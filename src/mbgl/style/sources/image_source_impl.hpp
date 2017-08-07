#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {

namespace style {

class ImageSource::Impl : public Source::Impl {
public:
    Impl(std::string id, std::array<LatLng, 4> coords);
    Impl(const Impl& rhs, std::array<LatLng, 4> coords);
    Impl(const Impl& rhs, PremultipliedImage&& image);

    ~Impl() final;

    std::shared_ptr<PremultipliedImage> getImage() const;
    std::array<LatLng, 4> getCoordinates() const;

    optional<std::string> getAttribution() const final;
private:
    std::array<LatLng, 4> coords;
    std::shared_ptr<PremultipliedImage> image;
};

} // namespace style
} // namespace mbgl
