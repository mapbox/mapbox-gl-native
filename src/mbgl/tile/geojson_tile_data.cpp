#include <mbgl/tile/geojson_tile_data.hpp>
#include <mbgl/tile/geojson_tile_source.hpp>
#include <mbgl/style/update_parameters.hpp>

namespace mbgl {

GeoJSONTileData::GeoJSONTileData(const OverscaledTileID& overscaledTileID,
                                 std::string sourceID,
                                 const style::UpdateParameters& parameters,
                                 mapbox::geojsonvt::GeoJSONVT* geojsonvt)
    : GeometryTileData(overscaledTileID, sourceID, parameters.style, parameters.mode) {
    tileSource = std::make_unique<GeoJSONTileSource>(*this, geojsonvt, overscaledTileID);
}

GeoJSONTileData::~GeoJSONTileData() = default;

void GeoJSONTileData::setNecessity(Necessity necessity) {
    tileSource->setNecessity(static_cast<TileSource::Necessity>(necessity));
}

} // namespace mbgl
