#include <mbgl/annotation/line_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/layer/line_layer.hpp>

namespace mbgl {

namespace geojsonvt = mapbox::geojsonvt;

LineAnnotationImpl::LineAnnotationImpl(const AnnotationID id_, const LineAnnotation& annotation_, const uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation(annotation_) {
}

void LineAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    std::unique_ptr<LineLayer> layer = std::make_unique<LineLayer>();
    layer->layout.lineJoin = LineJoinType::Round;

    layer->paint.lineOpacity = annotation.opacity;
    layer->paint.lineWidth = annotation.width;
    layer->paint.lineColor = annotation.color;

    layer->id = layerID;
    layer->source = AnnotationManager::SourceID;
    layer->sourceLayer = layer->id;

    style.addLayer(std::move(layer), AnnotationManager::PointLayerID);
}

const ShapeAnnotationGeometry& LineAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
