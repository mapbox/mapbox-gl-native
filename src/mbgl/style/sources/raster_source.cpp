#include <mbgl/style/sources/raster_source.hpp>

namespace mbgl {
namespace style {

RasterSource::RasterSource(std::string id_,
                           std::string url_,
                           uint16_t tileSize_,
                           std::unique_ptr<Tileset>&& tileset_,
                           std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : Source(SourceType::Raster, std::move(id_), std::move(url_), tileSize_, std::move(tileset_), std::move(geojsonvt_)) {
}

} // namespace style
} // namespace mbgl
