#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void SymbolLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool SymbolLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    paint.evaluate(parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    iconSize = layout.evaluate<IconSize>(parameters);
    textSize = layout.evaluate<TextSize>(parameters);
    
    auto hasIconOpacity = paint.evaluated.get<IconColor>().constantOr(Color::black()).a > 0 ||
        paint.evaluated.get<IconHaloColor>().constantOr(Color::black()).a > 0;
    auto hasTextOpacity = paint.evaluated.get<TextColor>().constantOr(Color::black()).a > 0 ||
        paint.evaluated.get<TextHaloColor>().constantOr(Color::black()).a > 0;
    
    passes = ((paint.evaluated.get<IconOpacity>().constantOr(1) > 0 && hasIconOpacity && iconSize > 0)
           || (paint.evaluated.get<TextOpacity>().constantOr(1) > 0 && hasTextOpacity && textSize > 0))
        ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> SymbolLayer::Impl::createBucket(const BucketParameters&, const std::vector<const Layer*>&) const {
    assert(false); // Should be calling createLayout() instead.
    return nullptr;
}

std::unique_ptr<SymbolLayout> SymbolLayer::Impl::createLayout(const BucketParameters& parameters,
                                                              const std::vector<const Layer*>& group,
                                                              const GeometryTileLayer& layer) const {
    return std::make_unique<SymbolLayout>(parameters,
                                          group,
                                          layer,
                                          *spriteAtlas);
}

IconPaintProperties::Evaluated SymbolLayer::Impl::iconPaintProperties() const {
    return IconPaintProperties::Evaluated {
        paint.evaluated.get<IconOpacity>(),
        paint.evaluated.get<IconColor>(),
        paint.evaluated.get<IconHaloColor>(),
        paint.evaluated.get<IconHaloWidth>(),
        paint.evaluated.get<IconHaloBlur>(),
        paint.evaluated.get<IconTranslate>(),
        paint.evaluated.get<IconTranslateAnchor>()
    };
}

TextPaintProperties::Evaluated SymbolLayer::Impl::textPaintProperties() const {
    return TextPaintProperties::Evaluated {
        paint.evaluated.get<TextOpacity>(),
        paint.evaluated.get<TextColor>(),
        paint.evaluated.get<TextHaloColor>(),
        paint.evaluated.get<TextHaloWidth>(),
        paint.evaluated.get<TextHaloBlur>(),
        paint.evaluated.get<TextTranslate>(),
        paint.evaluated.get<TextTranslateAnchor>()
    };
}


SymbolPropertyValues SymbolLayer::Impl::iconPropertyValues(const SymbolLayoutProperties::Evaluated& layout_) const {
    return SymbolPropertyValues {
        layout_.get<IconRotationAlignment>(), // icon-pitch-alignment is not yet implemented; inherit the rotation alignment
        layout_.get<IconRotationAlignment>(),
        layout_.get<IconSize>(),
        paint.evaluated.get<IconTranslate>(),
        paint.evaluated.get<IconTranslateAnchor>(),
        iconSize,
        1.0f,
        paint.evaluated.get<IconHaloColor>().constantOr(Color::black()).a > 0 &&
            paint.evaluated.get<IconHaloWidth>().constantOr(1),
        paint.evaluated.get<IconColor>().constantOr(Color::black()).a > 0
    };
}

SymbolPropertyValues SymbolLayer::Impl::textPropertyValues(const SymbolLayoutProperties::Evaluated& layout_) const {
    return SymbolPropertyValues {
        layout_.get<TextPitchAlignment>(),
        layout_.get<TextRotationAlignment>(),
        layout_.get<TextSize>(),
        paint.evaluated.get<TextTranslate>(),
        paint.evaluated.get<TextTranslateAnchor>(),
        textSize,
        24.0f,
        paint.evaluated.get<TextHaloColor>().constantOr(Color::black()).a > 0 &&
            paint.evaluated.get<TextHaloWidth>().constantOr(1),
        paint.evaluated.get<TextColor>().constantOr(Color::black()).a > 0
    };
}

} // namespace style
} // namespace mbgl
