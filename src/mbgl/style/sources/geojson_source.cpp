#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/tile/geojson_tile.hpp>

#include <mapbox/geojsonvt.hpp>

namespace mbgl {
namespace style {

GeoJSONSource::GeoJSONSource(std::string id_,
                             std::string url_,
                             uint16_t tileSize_,
                             std::unique_ptr<Tileset>&& tileset_,
                             std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : Source(SourceType::GeoJSON, std::move(id_), std::move(url_), tileSize_, std::move(tileset_)) {
    geojsonvt = std::move(geojsonvt_);
}

std::unique_ptr<Tile> GeoJSONSource::createTile(const OverscaledTileID& tileID,
                                                const UpdateParameters& parameters) {
    return std::make_unique<GeoJSONTile>(tileID, id, parameters, geojsonvt.get());
}

} // namespace style
} // namespace mbgl
