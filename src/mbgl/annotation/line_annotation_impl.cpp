#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>

namespace mbgl {

using namespace style;

LineAnnotationImpl::LineAnnotationImpl(AnnotationID id_, LineAnnotation annotation_)
    : ShapeAnnotationImpl(id_),
      annotation(ShapeAnnotationGeometry::visit(annotation_.geometry, CloseShapeAnnotation{}), annotation_.opacity, annotation_.width, annotation_.color) {
}

void LineAnnotationImpl::updateStyle(Style::Impl& style) const {
    Layer* layer = style.getLayer(layerID);

    if (!layer) {
        auto newLayer = std::make_unique<LineLayer>(layerID, AnnotationManager::SourceID);
        newLayer->setSourceLayer(layerID);
        newLayer->setLineJoin(LineJoinType::Round);
        layer = style.addLayer(std::move(newLayer), AnnotationManager::PointLayerID);
    }

    auto* lineLayer = layer->as<LineLayer>();
    lineLayer->setLineOpacity(annotation.opacity);
    lineLayer->setLineWidth(annotation.width);
    lineLayer->setLineColor(annotation.color);
}

const ShapeAnnotationGeometry& LineAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
