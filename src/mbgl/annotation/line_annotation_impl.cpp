#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/line_layer.hpp>

namespace mbgl {

using namespace style;

LineAnnotationImpl::LineAnnotationImpl(AnnotationID id_, LineAnnotation annotation_, uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation({ ShapeAnnotationGeometry::visit(annotation_.geometry, CloseShapeAnnotation{}), annotation_.opacity, annotation_.width, annotation_.color }) {
}

void LineAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    std::unique_ptr<LineLayer> layer = std::make_unique<LineLayer>(layerID, AnnotationManager::SourceID);
    layer->setSourceLayer(layerID);
    layer->setLineJoin(LineJoinType::Round);
    layer->setLineOpacity(annotation.opacity);
    layer->setLineWidth(annotation.width);
    layer->setLineColor(annotation.color);

    style.addLayer(std::move(layer), AnnotationManager::PointLayerID);
}

const ShapeAnnotationGeometry& LineAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
