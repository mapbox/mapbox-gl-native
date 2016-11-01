#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void SymbolLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool SymbolLayer::Impl::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    layout.iconSize.calculate(parameters);
    layout.textSize.calculate(parameters);
    iconSize = layout.iconSize;
    textSize = layout.textSize;

    passes = ((paint.iconOpacity > 0 && (paint.iconColor.value.a > 0 || paint.iconHaloColor.value.a > 0) && iconSize > 0)
           || (paint.textOpacity > 0 && (paint.textColor.value.a > 0 || paint.textHaloColor.value.a > 0) && textSize > 0))
        ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> SymbolLayer::Impl::createBucket(BucketParameters&) const {
    assert(false); // Should be calling createLayout() instead.
    return nullptr;
}

std::unique_ptr<SymbolLayout> SymbolLayer::Impl::createLayout(BucketParameters& parameters) const {
    SymbolLayoutProperties layoutProperties = layout;

    CalculationParameters p(parameters.tileID.overscaledZ);
    layoutProperties.recalculate(p);

    if (layoutProperties.iconRotationAlignment.value == AlignmentType::Auto) {
        if (layoutProperties.symbolPlacement.value == SymbolPlacementType::Line) {
            layoutProperties.iconRotationAlignment.value = AlignmentType::Map;
        } else {
            layoutProperties.iconRotationAlignment.value = AlignmentType::Viewport;
        }
    }

    if (layoutProperties.textRotationAlignment.value == AlignmentType::Auto) {
        if (layoutProperties.symbolPlacement.value == SymbolPlacementType::Line) {
            layoutProperties.textRotationAlignment.value = AlignmentType::Map;
        } else {
            layoutProperties.textRotationAlignment.value = AlignmentType::Viewport;
        }
    }

    // If unspecified `text-pitch-alignment` inherits `text-rotation-alignment`
    if (layoutProperties.textPitchAlignment.value == AlignmentType::Auto) {
        layoutProperties.textPitchAlignment.value = layoutProperties.textRotationAlignment.value;
    }

    layoutProperties.textSize.calculate(CalculationParameters(18));
    float textMaxSize = layoutProperties.textSize;

    layoutProperties.iconSize.calculate(CalculationParameters(p.z + 1));
    layoutProperties.textSize.calculate(CalculationParameters(p.z + 1));

    return std::make_unique<SymbolLayout>(id,
                                          parameters.layer.getName(),
                                          parameters.tileID.overscaleFactor(),
                                          parameters.tileID.overscaledZ,
                                          parameters.mode,
                                          parameters.layer,
                                          filter,
                                          layoutProperties,
                                          textMaxSize,
                                          *spriteAtlas);
}

SymbolPropertyValues SymbolLayer::Impl::iconPropertyValues(const SymbolLayoutProperties& layout_) const {
    return SymbolPropertyValues {
        layout_.iconRotationAlignment.value, // icon-pitch-alignment is not yet implemented; inherit the rotation alignment
        layout_.iconRotationAlignment.value,
        layout_.iconSize.value,
        paint.iconOpacity.value,
        paint.iconColor.value,
        paint.iconHaloColor.value,
        paint.iconHaloWidth.value,
        paint.iconHaloBlur.value,
        paint.iconTranslate.value,
        paint.iconTranslateAnchor.value,
        iconSize,
        1.0f
    };
}

SymbolPropertyValues SymbolLayer::Impl::textPropertyValues(const SymbolLayoutProperties& layout_) const {
    return SymbolPropertyValues {
        layout_.textPitchAlignment.value,
        layout_.textRotationAlignment.value,
        layout_.textSize.value,
        paint.textOpacity.value,
        paint.textColor.value,
        paint.textHaloColor.value,
        paint.textHaloWidth.value,
        paint.textHaloBlur.value,
        paint.textTranslate.value,
        paint.textTranslateAnchor.value,
        textSize,
        24.0f
    };
}

} // namespace style
} // namespace mbgl
