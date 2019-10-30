#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

GeoJSONSource::GeoJSONSource(const std::string& id, optional<GeoJSONOptions> options)
    : Source(makeMutable<Impl>(id, options)) {}

GeoJSONSource::~GeoJSONSource() = default;

const GeoJSONSource::Impl& GeoJSONSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

void GeoJSONSource::setURL(const std::string& url_) {
    url = url_;

    // Signal that the source description needs a reload
    if (loaded || req) {
        loaded = false;
        req.reset();
        observer->onSourceDescriptionChanged(*this);
    }
}

void GeoJSONSource::setGeoJSON(const mapbox::geojson::geojson& geoJSON) {
    setGeoJSONData(GeoJSONData::create(geoJSON, impl().getOptions()));
}

void GeoJSONSource::setGeoJSONData(std::shared_ptr<GeoJSONData> geoJSONData) {
    req.reset();
    baseImpl = makeMutable<Impl>(impl(), std::move(geoJSONData));
    observer->onSourceChanged(*this);
}

optional<std::string> GeoJSONSource::getURL() const {
    return url;
}

const GeoJSONOptions& GeoJSONSource::getOptions() const {
    return impl().getOptions();
}

void GeoJSONSource::loadDescription(FileSource& fileSource) {
    if (!url) {
        loaded = true;
        return;
    }

    if (req) {
        return;
    }

    req = fileSource.request(Resource::source(*url), [this](Response res) {
        if (res.error) {
            observer->onSourceError(
                *this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(
                *this, std::make_exception_ptr(std::runtime_error("unexpectedly empty GeoJSON")));
        } else {
            conversion::Error error;
            std::shared_ptr<GeoJSONData> geoJSONData;
            if (optional<GeoJSON> geoJSON = conversion::convertJSON<GeoJSON>(*res.data, error)) {
                geoJSONData = GeoJSONData::create(*geoJSON, impl().getOptions());
            } else {
                // Create an empty GeoJSON VT object to make sure we're not infinitely waiting for tiles to load.
                Log::Error(Event::ParseStyle, "Failed to parse GeoJSON data: %s",
                           error.message.c_str());
            }
            baseImpl = makeMutable<Impl>(impl(), std::move(geoJSONData));
            loaded = true;
            observer->onSourceLoaded(*this);
        }
    });
}

bool GeoJSONSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return mbgl::underlying_type(Tile::Kind::Geometry) == mbgl::underlying_type(info->tileKind);
}

} // namespace style
} // namespace mbgl
