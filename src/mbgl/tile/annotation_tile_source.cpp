#include <mbgl/tile/annotation_tile_source.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/async_request.hpp>

namespace mbgl {

AnnotationTileSource::AnnotationTileSource(GeometryTileData& tileData_,
                                           const OverscaledTileID& tileID_,
                                           AnnotationManager& annotationManager_)
    : GeometryTileSource(tileData_), tileID(tileID_), annotationManager(annotationManager_) {
    annotationManager.addTileSource(*this);
}

AnnotationTileSource::~AnnotationTileSource() {
    annotationManager.removeTileSource(*this);
}

void AnnotationTileSource::update(std::unique_ptr<GeometryTile> tile) {
    tileData.setData(nullptr, std::move(tile), {}, {});
}

} // namespace mbgl
