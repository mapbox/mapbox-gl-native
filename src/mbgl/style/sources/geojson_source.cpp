#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>

namespace mbgl {
namespace style {

GeoJSONSource::GeoJSONSource(const std::string& id)
    : Source(SourceType::GeoJSON, std::make_unique<GeoJSONSource::Impl>(std::move(id), *this))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

void GeoJSONSource::setURL(const std::string& url) {
    impl->setURL(url);
}

void GeoJSONSource::setGeoJSON(GeoJSON&& geoJSON) {
    impl->setGeoJSON(std::move(geoJSON));
}

} // namespace style
} // namespace mbgl
