// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>

namespace mbgl {
namespace style {

FillExtrusionLayerProperties::FillExtrusionLayerProperties(
    Immutable<FillExtrusionLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

FillExtrusionLayerProperties::FillExtrusionLayerProperties(
    Immutable<FillExtrusionLayer::Impl> impl_,
    CrossfadeParameters crossfade_,
    FillExtrusionPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    crossfade(std::move(crossfade_)),
    evaluated(std::move(evaluated_)) {}

FillExtrusionLayerProperties::~FillExtrusionLayerProperties() = default;

unsigned long FillExtrusionLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const FillExtrusionLayer::Impl& FillExtrusionLayerProperties::layerImpl() const {
    return static_cast<const FillExtrusionLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl
