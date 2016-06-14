#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

using namespace style;

FillAnnotationImpl::FillAnnotationImpl(const AnnotationID id_, const FillAnnotation& annotation_, const uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation(annotation_) {
}

void FillAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    std::unique_ptr<FillLayer> layer = std::make_unique<FillLayer>(layerID);
    layer->setSource(AnnotationManager::SourceID, layerID);

    layer->setFillOpacity(annotation.opacity);
    layer->setFillColor(annotation.color);
    layer->setFillOutlineColor(annotation.outlineColor);

    style.addLayer(std::move(layer), AnnotationManager::PointLayerID);
}

const ShapeAnnotationGeometry& FillAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
