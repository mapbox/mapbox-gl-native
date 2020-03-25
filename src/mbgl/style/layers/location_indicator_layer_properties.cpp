// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/location_indicator_layer_properties.hpp>

#include <mbgl/style/layers/location_indicator_layer_impl.hpp>

namespace mbgl {
namespace style {

LocationIndicatorLayerProperties::LocationIndicatorLayerProperties(
    Immutable<LocationIndicatorLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

LocationIndicatorLayerProperties::LocationIndicatorLayerProperties(
    Immutable<LocationIndicatorLayer::Impl> impl_,
    LocationIndicatorPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

LocationIndicatorLayerProperties::~LocationIndicatorLayerProperties() = default;

unsigned long LocationIndicatorLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const LocationIndicatorLayer::Impl& LocationIndicatorLayerProperties::layerImpl() const {
    return static_cast<const LocationIndicatorLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl

// clang-format on
