// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/hillshade_layer_properties.hpp>

#include <mbgl/style/layers/hillshade_layer_impl.hpp>

namespace mbgl {
namespace style {

HillshadeLayerProperties::HillshadeLayerProperties(
    Immutable<HillshadeLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

HillshadeLayerProperties::HillshadeLayerProperties(
    Immutable<HillshadeLayer::Impl> impl_,
    HillshadePaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

HillshadeLayerProperties::~HillshadeLayerProperties() = default;

unsigned long HillshadeLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const HillshadeLayer::Impl& HillshadeLayerProperties::layerImpl() const {
    return static_cast<const HillshadeLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl

// clang-format on
