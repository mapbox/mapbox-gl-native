#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>

namespace mbgl {
namespace style {

bool LocationIndicatorLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

} // namespace style
} // namespace mbgl
