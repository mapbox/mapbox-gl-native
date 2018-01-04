#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {
namespace style {

ImageSource::ImageSource(std::string id, const std::array<LatLng, 4> coords_)
    : Source(makeMutable<Impl>(std::move(id), coords_)) {
}

ImageSource::~ImageSource() = default;

const ImageSource::Impl& ImageSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

void ImageSource::setCoordinates(const std::array<LatLng, 4>& coords_) {
    baseImpl = makeMutable<Impl>(impl(), coords_);
    observer->onSourceChanged(*this);
}

std::array<LatLng, 4> ImageSource::getCoordinates() const {
    return impl().getCoordinates();
}

void ImageSource::setURL(const std::string& url_) {
    url = std::move(url_);
    // Signal that the source description needs a reload
    if (loaded || req) {
        loaded = false;
        req.reset();
        observer->onSourceDescriptionChanged(*this);
    }
}

void ImageSource::setImage(PremultipliedImage&& image_) {
    url = {};
    if (req) {
        req.reset();
    }
    loaded = true;
    baseImpl = makeMutable<Impl>(impl(), std::move(image_));
    observer->onSourceChanged(*this);
}

optional<std::string> ImageSource::getURL() const {
    return url;
}

void ImageSource::loadDescription(FileSource& fileSource) {
    if (!url) {
        loaded = true;
    }

    if (req ||  loaded) {
        return;
    }
    const Resource imageResource { Resource::Image, *url, {} };

    req = fileSource.request(imageResource, [this](Response res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty image url")));
        } else {
            try {
                baseImpl = makeMutable<Impl>(impl(), decodeImage(*res.data));
            } catch (...) {
                observer->onSourceError(*this, std::current_exception());
            }
            loaded = true;
            observer->onSourceLoaded(*this);
        }
    });
}

} // namespace style
} // namespace mbgl
