#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

bool SymbolLayer::Impl::hasFormatSectionOverrides() const {
    if (!hasFormatSectionOverrides_) {
        hasFormatSectionOverrides_ = SymbolLayerPaintPropertyOverrides::hasOverrides(layout.get<TextField>());
    }
    return *hasFormatSectionOverrides_;
}

bool SymbolLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(other.getTypeInfo() == getTypeInfo());
    const auto& impl = static_cast<const style::SymbolLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           layout     != impl.layout ||
           paint.hasDataDrivenPropertyDifference(impl.paint) ||
           (hasFormatSectionOverrides() && SymbolLayerPaintPropertyOverrides::hasPaintPropertyDifference(paint, impl.paint));
}

void SymbolLayer::Impl::populateFontStack(std::set<FontStack>& fontStack) const {
    if (layout.get<TextField>().isUndefined()) {
        return;
    }

    layout.get<TextFont>().match(
        [&fontStack] (Undefined) {
            fontStack.insert({"Open Sans Regular", "Arial Unicode MS Regular"});
        },
        [&fontStack] (const FontStack& constant) {
            fontStack.insert(constant);
        },
        [&] (const auto& function) {
            for (const auto& value : function.possibleOutputs()) {
                if (value) {
                    fontStack.insert(*value);
                } else {
                    Log::Warning(Event::ParseStyle, "Layer '%s' has an invalid value for text-font and will not render text. Output values must be contained as literals within the expression.", id.c_str());
                    break;
                }
            }
        }
    );
}

} // namespace style
} // namespace mbgl
