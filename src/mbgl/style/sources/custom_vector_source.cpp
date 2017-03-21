#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/style/sources/custom_vector_source_impl.hpp>

namespace mbgl {
namespace style {

CustomVectorSource::CustomVectorSource(std::string id,
                                       const GeoJSONOptions options,
                                       std::function<void(const CanonicalTileID&)> fetchTile)
    : Source(SourceType::Vector,
             std::make_unique<CustomVectorSource::Impl>(std::move(id), *this, options, fetchTile)),
      impl(static_cast<Impl*>(baseImpl.get())) {
}

void CustomVectorSource::setTileData(const CanonicalTileID& tileId,
                                     const mapbox::geojson::geojson& geoJSON) {
    impl->setTileData(tileId, geoJSON);
}

void CustomVectorSource::reloadRegion(mbgl::LatLngBounds bounds, uint8_t z) {
    impl->reloadRegion(bounds, z);
}

void CustomVectorSource::reloadTile(const CanonicalTileID& tileId) {
    impl->reloadTile(tileId);
}

void CustomVectorSource::reload() {
    impl->reload();
}

} // namespace style
} // namespace mbgl
