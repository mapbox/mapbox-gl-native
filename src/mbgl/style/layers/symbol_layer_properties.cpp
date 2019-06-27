// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer_properties.hpp>

#include <mbgl/style/layers/symbol_layer_impl.hpp>

namespace mbgl {
namespace style {

SymbolLayerProperties::SymbolLayerProperties(
    Immutable<SymbolLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

SymbolLayerProperties::SymbolLayerProperties(
    Immutable<SymbolLayer::Impl> impl_,
    SymbolPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

SymbolLayerProperties::~SymbolLayerProperties() = default;

unsigned long SymbolLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const SymbolLayer::Impl& SymbolLayerProperties::layerImpl() const {
    return static_cast<const SymbolLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl
