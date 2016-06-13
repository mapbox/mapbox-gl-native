#include <mbgl/style/sources/vector_source.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id_,
                           std::string url_,
                           uint16_t tileSize_,
                           std::unique_ptr<Tileset>&& tileset_,
                           std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : Source(SourceType::Vector, std::move(id_), std::move(url_), tileSize_, std::move(tileset_), std::move(geojsonvt_)) {
}

} // namespace style
} // namespace mbgl
