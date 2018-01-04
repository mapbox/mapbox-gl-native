#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>

namespace mbgl {
namespace style {

bool FillExtrusionLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(dynamic_cast<const FillExtrusionLayer::Impl*>(&other));
    const auto& impl = static_cast<const style::FillExtrusionLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

} // namespace style
} // namespace mbgl
