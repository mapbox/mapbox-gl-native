// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer_properties.hpp>

#include <mbgl/style/layers/line_layer_impl.hpp>

namespace mbgl {
namespace style {

LineLayerProperties::LineLayerProperties(
    Immutable<LineLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

LineLayerProperties::LineLayerProperties(
    Immutable<LineLayer::Impl> impl_,
    CrossfadeParameters crossfade_,
    LinePaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    crossfade(std::move(crossfade_)),
    evaluated(std::move(evaluated_)) {}

LineLayerProperties::~LineLayerProperties() = default;

unsigned long LineLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const LineLayer::Impl& LineLayerProperties::layerImpl() const {
    return static_cast<const LineLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl
