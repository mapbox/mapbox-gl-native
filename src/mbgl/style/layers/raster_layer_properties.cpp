// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer_properties.hpp>

#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {
namespace style {

RasterLayerProperties::RasterLayerProperties(
    Immutable<RasterLayer::Impl> impl_)
    : LayerProperties(std::move(impl_)) {}

RasterLayerProperties::RasterLayerProperties(
    Immutable<RasterLayer::Impl> impl_,
    RasterPaintProperties::PossiblyEvaluated evaluated_)
  : LayerProperties(std::move(impl_)),
    evaluated(std::move(evaluated_)) {}

RasterLayerProperties::~RasterLayerProperties() = default;

unsigned long RasterLayerProperties::constantsMask() const {
    return evaluated.constantsMask();
}

const RasterLayer::Impl& RasterLayerProperties::layerImpl() const {
    return static_cast<const RasterLayer::Impl&>(*baseImpl);
}

} // namespace style
} // namespace mbgl

// clang-format on
