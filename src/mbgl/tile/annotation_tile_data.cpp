#include <mbgl/tile/annotation_tile_data.hpp>
#include <mbgl/tile/annotation_tile_source.hpp>
#include <mbgl/style/update_parameters.hpp>

namespace mbgl {

AnnotationTileData::AnnotationTileData(const OverscaledTileID& overscaledTileID,
                                       std::string sourceID,
                                       const style::UpdateParameters& parameters)
    : GeometryTileData(overscaledTileID, sourceID, parameters.style, parameters.mode) {
    tileSource = std::make_unique<AnnotationTileSource>(*this, overscaledTileID, parameters.annotationManager);
}

AnnotationTileData::~AnnotationTileData() = default;

void AnnotationTileData::setNecessity(Necessity necessity) {
    tileSource->setNecessity(static_cast<TileSource::Necessity>(necessity));
}

} // namespace mbgl
