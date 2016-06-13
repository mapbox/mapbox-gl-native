#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>

namespace mbgl {

AnnotationSource::AnnotationSource()
    : Source(SourceType::Annotations, AnnotationManager::SourceID, "", util::tileSize, std::make_unique<Tileset>()) {
}

Range<uint8_t> AnnotationSource::getZoomRange() {
    return { 0, 22 };
}

void AnnotationSource::load(FileSource&) {
    loaded = true;
}

std::unique_ptr<Tile> AnnotationSource::createTile(const OverscaledTileID& tileID,
                                                   const style::UpdateParameters& parameters) {
    return std::make_unique<AnnotationTile>(tileID, id, parameters);
}

} // namespace mbgl
