#include <mbgl/annotation/annotation_source_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>

#include <boost/function_output_iterator.hpp>

namespace mbgl {

using namespace style;

std::unique_ptr<AnnotationTileData> AnnotationData::getTile(const CanonicalTileID& tileID) const {
    if (symbolAnnotations.empty() && shapeAnnotations.empty())
        return nullptr;

    auto tileData = std::make_unique<AnnotationTileData>();

    AnnotationTileLayer& pointLayer = tileData->layers.emplace(
            AnnotationManager::PointLayerID,
            AnnotationManager::PointLayerID
    ).first->second;

    LatLngBounds tileBounds(tileID);

    symbolTree.query(boost::geometry::index::intersects(tileBounds),
                     boost::make_function_output_iterator([&](const auto& val){
                         val->updateTileLayer(tileID, pointLayer);
                     }));

    for (const auto& shape : shapeAnnotations) {
        shape.second->updateTileData(tileID, *tileData);
    }

    return tileData;
}

AnnotationSource::Impl::Impl()
    : Source::Impl(SourceType::Annotations, AnnotationManager::SourceID) {

}

const AnnotationData* AnnotationSource::Impl::getData() const {
    return &data;
}

AnnotationData* AnnotationSource::Impl::getData() {
    return &data;
}

optional<std::string> AnnotationSource::Impl::getAttribution() const {
    return {};
}

} // namespace mbgl
