#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

using namespace style;

FillAnnotationImpl::FillAnnotationImpl(AnnotationID id_, FillAnnotation annotation_, uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation({ ShapeAnnotationGeometry::visit(annotation_.geometry, CloseShapeAnnotation{}), annotation_.opacity, annotation_.color, annotation_.outlineColor }) {
}

void FillAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    std::unique_ptr<FillLayer> layer = std::make_unique<FillLayer>(layerID, AnnotationManager::SourceID);
    layer->setSourceLayer(layerID);
    layer->setFillOpacity(annotation.opacity);
    layer->setFillColor(annotation.color);
    layer->setFillOutlineColor(annotation.outlineColor);

    style.addLayer(std::move(layer), AnnotationManager::PointLayerID);
}

const ShapeAnnotationGeometry& FillAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
