#include <mbgl/annotation/style_sourced_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

using namespace style;

StyleSourcedAnnotationImpl::StyleSourcedAnnotationImpl(AnnotationID id_, StyleSourcedAnnotation annotation_, uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation(std::move(annotation_)) {
}

void StyleSourcedAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    const Layer* sourceLayer = style.getLayer(annotation.layerID);
    if (!sourceLayer)
        return;

    if (sourceLayer->is<LineLayer>()) {
        std::unique_ptr<Layer> layer = sourceLayer->baseImpl->copy(layerID, AnnotationManager::SourceID);
        layer->as<LineLayer>()->setSourceLayer(layerID);
        layer->as<LineLayer>()->setVisibility(VisibilityType::Visible);
        style.addLayer(std::move(layer), sourceLayer->getID());
    } else if (sourceLayer->is<FillLayer>()) {
        std::unique_ptr<Layer> layer = sourceLayer->baseImpl->copy(layerID, AnnotationManager::SourceID);
        layer->as<FillLayer>()->setSourceLayer(layerID);
        layer->as<FillLayer>()->setVisibility(VisibilityType::Visible);
        style.addLayer(std::move(layer), sourceLayer->getID());
    }
}

const ShapeAnnotationGeometry& StyleSourcedAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
