#include <mbgl/style/layers/fill_layer_impl.hpp>

namespace mbgl {
namespace style {

bool FillLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(dynamic_cast<const FillLayer::Impl*>(&other));
    const auto& impl = static_cast<const style::FillLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

} // namespace style
} // namespace mbgl
