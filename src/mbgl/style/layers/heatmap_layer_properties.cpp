// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/heatmap_layer_properties.hpp>

#include <mbgl/style/layers/heatmap_layer_impl.hpp>

namespace mbgl {
namespace style {

HeatmapLayerProperties::HeatmapLayerProperties(
    Immutable<HeatmapLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

HeatmapLayerProperties::HeatmapLayerProperties(
    Immutable<HeatmapLayer::Impl> impl_,
    HeatmapPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

HeatmapLayerProperties::~HeatmapLayerProperties() = default;

unsigned long HeatmapLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const HeatmapLayer::Impl& HeatmapLayerProperties::layerImpl() const {
    return static_cast<const HeatmapLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl
