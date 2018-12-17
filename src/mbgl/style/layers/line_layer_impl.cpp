#include <mbgl/style/layers/line_layer_impl.hpp>

namespace mbgl {
namespace style {

bool LineLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(other.getTypeInfo() == getTypeInfo());
    const auto& impl = static_cast<const style::LineLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           layout     != impl.layout ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

} // namespace style
} // namespace mbgl
