#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {
class LatLng;

namespace style {

class ImageSource::Impl : public Source::Impl {
public:
    Impl(std::string id, std::vector<LatLng> coords);
    Impl(const Impl& rhs, std::vector<LatLng> coords);
    Impl(const Impl& rhs, UnassociatedImage image);

    ~Impl() final;

    void setImage(UnassociatedImage );
    const mbgl::UnassociatedImage& getImage() const;
    std::vector<LatLng> getCoordinates() const;

    optional<std::string> getAttribution() const final;
private:
    std::vector<LatLng> coords;
    mbgl::UnassociatedImage image;
};

} // namespace style
} // namespace mbgl
