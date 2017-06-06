#include <mbgl/annotation/fill_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

using namespace style;

FillAnnotationImpl::FillAnnotationImpl(const AnnotationID& id_,
                                       const FillAnnotation &annotation_,
                                       const uint8_t maxZoom_)
        : ShapeAnnotationImpl(id_, maxZoom_)
        , opacity(annotation_.opacity)
        , color(annotation_.color)
        , outlineColor(annotation_.outlineColor) {
}

void FillAnnotationImpl::updateStyle(Style& style) const {
    Layer* layer = style.getLayer(layerID);

    if (!layer) {
        auto newLayer = std::make_unique<FillLayer>(layerID, AnnotationManager::SourceID);
        newLayer->setSourceLayer(layerID);
        layer = style.addLayer(std::move(newLayer), AnnotationManager::PointLayerID);
    }

    auto *fillLayer = layer->as<FillLayer>();
    fillLayer->setFillOpacity(opacity);
    fillLayer->setFillColor(color);
    fillLayer->setFillOutlineColor(outlineColor);
}

} // namespace mbgl
