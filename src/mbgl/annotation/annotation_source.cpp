#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/annotation_manager.hpp>

namespace mbgl {

AnnotationSource::AnnotationSource()
    : Source(SourceType::Annotations, AnnotationManager::SourceID, "", util::tileSize, std::make_unique<Tileset>()) {
}

} // namespace mbgl
