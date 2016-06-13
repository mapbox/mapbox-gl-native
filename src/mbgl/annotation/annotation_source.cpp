#include <mbgl/annotation/annotation_source.hpp>

namespace mbgl {

AnnotationSource::AnnotationSource(std::string id_,
                                   std::string url_,
                                   uint16_t tileSize_,
                                   std::unique_ptr<Tileset>&& tileset_,
                                   std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : Source(SourceType::Annotations, std::move(id_), std::move(url_), tileSize_, std::move(tileset_), std::move(geojsonvt_)) {
}

} // namespace mbgl
