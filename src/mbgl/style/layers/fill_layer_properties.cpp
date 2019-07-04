// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <mbgl/style/layers/fill_layer_impl.hpp>

namespace mbgl {
namespace style {

FillLayerProperties::FillLayerProperties(
    Immutable<FillLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

FillLayerProperties::FillLayerProperties(
    Immutable<FillLayer::Impl> impl_,
    CrossfadeParameters crossfade_,
    FillPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    crossfade(std::move(crossfade_)),
    evaluated(std::move(evaluated_)) {}

FillLayerProperties::~FillLayerProperties() = default;

unsigned long FillLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const FillLayer::Impl& FillLayerProperties::layerImpl() const {
    return static_cast<const FillLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl
