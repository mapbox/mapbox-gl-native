// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/location_component_layer_properties.hpp>

#include <mbgl/style/layers/location_component_layer_impl.hpp>

namespace mbgl {
namespace style {

LocationComponentLayerProperties::LocationComponentLayerProperties(
    Immutable<LocationComponentLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

LocationComponentLayerProperties::LocationComponentLayerProperties(
    Immutable<LocationComponentLayer::Impl> impl_,
    LocationComponentPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

LocationComponentLayerProperties::~LocationComponentLayerProperties() = default;

unsigned long LocationComponentLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const LocationComponentLayer::Impl& LocationComponentLayerProperties::layerImpl() const {
    return static_cast<const LocationComponentLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl

// clang-format on
