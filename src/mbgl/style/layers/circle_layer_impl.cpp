#include <mbgl/style/layers/circle_layer_impl.hpp>

namespace mbgl {
namespace style {

bool CircleLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(dynamic_cast<const CircleLayer::Impl*>(&other));
    const auto& impl = static_cast<const style::CircleLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

} // namespace style
} // namespace mbgl
