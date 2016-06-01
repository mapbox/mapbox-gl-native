#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/layer/fill_layer.hpp>

namespace mbgl {

namespace geojsonvt = mapbox::geojsonvt;

FillAnnotationImpl::FillAnnotationImpl(const AnnotationID id_, const FillAnnotation& annotation_, const uint8_t maxZoom_)
    : ShapeAnnotationImpl(id_, maxZoom_),
      annotation(annotation_) {
}

void FillAnnotationImpl::updateStyle(Style& style) const {
    if (style.getLayer(layerID))
        return;

    std::unique_ptr<FillLayer> layer = std::make_unique<FillLayer>();

    layer->paint.fillOpacity = annotation.opacity;
    layer->paint.fillColor = annotation.color;
    layer->paint.fillOutlineColor = annotation.outlineColor;

    layer->id = layerID;
    layer->source = AnnotationManager::SourceID;
    layer->sourceLayer = layer->id;

    style.addLayer(std::move(layer), AnnotationManager::PointLayerID);
}

const Geometry<double>& FillAnnotationImpl::geometry() const {
    return annotation.geometry;
}

} // namespace mbgl
