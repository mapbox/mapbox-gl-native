#include <mbgl/style/layers/location_component_layer.hpp>
#include <mbgl/style/layers/location_component_layer_impl.hpp>

namespace mbgl {
namespace style {

bool LocationComponentLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

} // namespace style
} // namespace mbgl
