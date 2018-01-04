#include <mbgl/style/layers/line_layer_impl.hpp>

namespace mbgl {
namespace style {

bool LineLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(dynamic_cast<const LineLayer::Impl*>(&other));
    const auto& impl = static_cast<const style::LineLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           layout     != impl.layout ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

} // namespace style
} // namespace mbgl
