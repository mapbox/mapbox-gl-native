#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>

namespace mbgl {
namespace style {

GeoJSONSource::GeoJSONSource(const std::string& id, const GeoJSONOptions& options)
    : Source(SourceType::GeoJSON,
             std::make_unique<GeoJSONSource::Impl>(std::move(id), *this, options)),
      impl(static_cast<Impl*>(baseImpl.get())) {
}

void GeoJSONSource::setURL(const std::string& url) {
    impl->setURL(url);
}

void GeoJSONSource::setGeoJSON(const mapbox::geojson::geojson& geoJSON) {
    impl->setGeoJSON(geoJSON);
}

optional<std::string> GeoJSONSource::getURL() const {
    return impl->getURL();
}

} // namespace style
} // namespace mbgl
