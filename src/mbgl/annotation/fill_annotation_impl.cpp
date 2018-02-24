#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

using namespace style;

FillAnnotationImpl::FillAnnotationImpl(AnnotationID id_, FillAnnotation annotation_)
    : ShapeAnnotationImpl(id_),
      annotation(ShapeAnnotationGeometry::visit(annotation_.geometry, CloseShapeAnnotation{}), annotation_.opacity, annotation_.color, annotation_.outlineColor) {
}

void FillAnnotationImpl::updateStyle(Style::Impl& style) const {
    Layer* layer = style.getLayer(layerID);

    if (!layer) {
        auto newLayer = std::make_unique<FillLayer>(layerID, AnnotationManager::SourceID);
        newLayer->setSourceLayer(layerID);
        layer = style.addLayer(std::move(newLayer), AnnotationManager::PointLayerID);
    }

    auto* fillLayer = layer->as<FillLayer>();
    fillLayer->setFillOpacity(annotation.opacity);
    fillLayer->setFillColor(annotation.color);
    fillLayer->setFillOutlineColor(annotation.outlineColor);
}

const ShapeAnnotationGeometry& FillAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
