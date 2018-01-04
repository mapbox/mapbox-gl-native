#include <mbgl/style/layers/symbol_layer_impl.hpp>

namespace mbgl {
namespace style {

bool SymbolLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(dynamic_cast<const SymbolLayer::Impl*>(&other));
    const auto& impl = static_cast<const style::SymbolLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           layout     != impl.layout ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

} // namespace style
} // namespace mbgl
