#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/bucket_parameters.hpp>
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

    passes = ((paint.evaluated.get<IconOpacity>() > 0 && (paint.evaluated.get<IconColor>().a > 0 || paint.evaluated.get<IconHaloColor>().a > 0) && iconSize > 0)
           || (paint.evaluated.get<TextOpacity>() > 0 && (paint.evaluated.get<TextColor>().a > 0 || paint.evaluated.get<TextHaloColor>().a > 0) && textSize > 0))
        ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> SymbolLayer::Impl::createBucket(BucketParameters&, const GeometryTileLayer&) const {
    assert(false); // Should be calling createLayout() instead.
    return nullptr;
}

std::unique_ptr<SymbolLayout> SymbolLayer::Impl::createLayout(BucketParameters& parameters,
                                                              const GeometryTileLayer& layer,
                                                              std::vector<std::string> group) const {
    PropertyEvaluationParameters p(parameters.tileID.overscaledZ);
    SymbolLayoutProperties::Evaluated evaluated = layout.evaluate(p);

    if (evaluated.get<IconRotationAlignment>() == AlignmentType::Auto) {
        if (evaluated.get<SymbolPlacement>() == SymbolPlacementType::Line) {
            evaluated.get<IconRotationAlignment>() = AlignmentType::Map;
        } else {
            evaluated.get<IconRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    if (evaluated.get<TextRotationAlignment>() == AlignmentType::Auto) {
        if (evaluated.get<SymbolPlacement>() == SymbolPlacementType::Line) {
            evaluated.get<TextRotationAlignment>() = AlignmentType::Map;
        } else {
            evaluated.get<TextRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    // If unspecified `text-pitch-alignment` inherits `text-rotation-alignment`
    if (evaluated.get<TextPitchAlignment>() == AlignmentType::Auto) {
        evaluated.get<TextPitchAlignment>() = evaluated.get<TextRotationAlignment>();
    }

    float textMaxSize = layout.evaluate<TextSize>(PropertyEvaluationParameters(18));

    evaluated.get<IconSize>() = layout.evaluate<IconSize>(PropertyEvaluationParameters(p.z + 1));
    evaluated.get<TextSize>() = layout.evaluate<TextSize>(PropertyEvaluationParameters(p.z + 1));

    return std::make_unique<SymbolLayout>(std::move(group),
                                          layer.getName(),
                                          parameters.tileID.overscaleFactor(),
                                          parameters.tileID.overscaledZ,
                                          parameters.mode,
                                          layer,
                                          filter,
                                          evaluated,
                                          textMaxSize,
                                          *spriteAtlas);
}

SymbolPropertyValues SymbolLayer::Impl::iconPropertyValues(const SymbolLayoutProperties::Evaluated& layout_) const {
    return SymbolPropertyValues {
        layout_.get<IconRotationAlignment>(), // icon-pitch-alignment is not yet implemented; inherit the rotation alignment
        layout_.get<IconRotationAlignment>(),
        layout_.get<IconSize>(),
        paint.evaluated.get<IconOpacity>(),
        paint.evaluated.get<IconColor>(),
        paint.evaluated.get<IconHaloColor>(),
        paint.evaluated.get<IconHaloWidth>(),
        paint.evaluated.get<IconHaloBlur>(),
        paint.evaluated.get<IconTranslate>(),
        paint.evaluated.get<IconTranslateAnchor>(),
        iconSize,
        1.0f
    };
}

SymbolPropertyValues SymbolLayer::Impl::textPropertyValues(const SymbolLayoutProperties::Evaluated& layout_) const {
    return SymbolPropertyValues {
        layout_.get<TextPitchAlignment>(),
        layout_.get<TextRotationAlignment>(),
        layout_.get<TextSize>(),
        paint.evaluated.get<TextOpacity>(),
        paint.evaluated.get<TextColor>(),
        paint.evaluated.get<TextHaloColor>(),
        paint.evaluated.get<TextHaloWidth>(),
        paint.evaluated.get<TextHaloBlur>(),
        paint.evaluated.get<TextTranslate>(),
        paint.evaluated.get<TextTranslateAnchor>(),
        textSize,
        24.0f
    };
}

} // namespace style
} // namespace mbgl
