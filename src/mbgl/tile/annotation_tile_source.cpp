#include <mbgl/tile/annotation_tile_source.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/async_request.hpp>

namespace mbgl {

AnnotationTileSource::AnnotationTileSource(const OverscaledTileID& tileID_,
                                             AnnotationManager& annotationManager_)
    : tileID(tileID_), annotationManager(annotationManager_) {
}

AnnotationTileSource::~AnnotationTileSource() {
    annotationManager.removeTileSource(*this);
}

std::unique_ptr<AsyncRequest>
AnnotationTileSource::monitorTile(const GeometryTileSource::Callback& callback_) {
    callback = callback_;
    annotationManager.addTileSource(*this);
    return nullptr;
}

void AnnotationTileSource::update(std::unique_ptr<GeometryTile> tile) {
    callback(nullptr, std::move(tile), {}, {});
}

} // namespace mbgl
