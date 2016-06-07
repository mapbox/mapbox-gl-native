#include <mbgl/tile/annotation_tile_data.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

namespace mbgl {

AnnotationTileData::AnnotationTileData(const OverscaledTileID& overscaledTileID,
                                       std::string sourceID,
                                       const style::UpdateParameters& parameters)
    : GeometryTileData(overscaledTileID, sourceID, parameters.style, parameters.mode),
      annotationManager(parameters.annotationManager) {
    annotationManager.addTileData(*this);
}

AnnotationTileData::~AnnotationTileData() {
    annotationManager.removeTileData(*this);
}

void AnnotationTileData::setNecessity(Necessity) {}

} // namespace mbgl
