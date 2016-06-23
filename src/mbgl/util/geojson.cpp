#include <mbgl/util/geojson.hpp>

#include <mapbox/geojsonvt.hpp>

namespace mbgl {

GeoJSON::GeoJSON(std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> impl_) : impl(std::move(impl_)) {}
GeoJSON::GeoJSON(GeoJSON&&) = default;
GeoJSON::~GeoJSON() = default;

} // namespace mbgl
