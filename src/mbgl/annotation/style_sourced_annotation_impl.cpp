#include <mbgl/annotation/style_sourced_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>

namespace mbgl {

namespace geojsonvt = mapbox::geojsonvt;

StyleSourcedAnnotationImpl::StyleSourcedAnnotationImpl(const AnnotationID id_, const StyleSourcedAnnotation& annotation_, const uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation(annotation_) {
}

void StyleSourcedAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    const StyleLayer* sourceLayer = style.getLayer(annotation.layerID);
    if (!sourceLayer)
        return;

    std::unique_ptr<StyleLayer> layer = sourceLayer->clone();

    layer->id = layerID;
    layer->ref = "";
    layer->source = AnnotationManager::SourceID;
    layer->sourceLayer = layer->id;
    layer->visibility = VisibilityType::Visible;

    style.addLayer(std::move(layer), sourceLayer->id);
}

const ShapeAnnotationGeometry& StyleSourcedAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
