// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer_properties.hpp>

#include <mbgl/style/layers/circle_layer_impl.hpp>

namespace mbgl {
namespace style {

CircleLayerProperties::CircleLayerProperties(
    Immutable<CircleLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

CircleLayerProperties::CircleLayerProperties(
    Immutable<CircleLayer::Impl> impl_,
    CirclePaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

CircleLayerProperties::~CircleLayerProperties() = default;

unsigned long CircleLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const CircleLayer::Impl& CircleLayerProperties::layerImpl() const {
    return static_cast<const CircleLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl

// clang-format on
