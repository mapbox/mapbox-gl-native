#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/bucket_parameters.hpp>

namespace mbgl {
namespace style {

void SymbolLayer::Impl::parseLayout(const JSValue& value) {
    layout.parse(value);
}

void SymbolLayer::Impl::parsePaints(const JSValue& layer) {
    paint.parse(layer);
}

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

std::unique_ptr<Bucket> SymbolLayer::Impl::createBucket(BucketParameters& parameters) const {
    auto bucket = std::make_unique<SymbolBucket>(parameters.tileID.overscaleFactor(),
                                                 parameters.tileID.overscaledZ,
                                                 parameters.mode,
                                                 id,
                                                 parameters.layer.getName());

    bucket->layout = layout;

    CalculationParameters p(parameters.tileID.overscaledZ);
    bucket->layout.symbolPlacement.calculate(p);
    if (bucket->layout.symbolPlacement.value == SymbolPlacementType::Line) {
        bucket->layout.iconRotationAlignment.value = AlignmentType::Map;
        bucket->layout.textRotationAlignment.value = AlignmentType::Map;
    };

    // If unspecified `text-pitch-alignment` inherits `text-rotation-alignment`
    if (bucket->layout.textPitchAlignment.value == AlignmentType::Undefined) {
        bucket->layout.textPitchAlignment.value = bucket->layout.textRotationAlignment.value;
    };

    bucket->layout.recalculate(p);

    bucket->layout.iconSize.calculate(CalculationParameters(18));
    bucket->layout.textSize.calculate(CalculationParameters(18));
    bucket->iconMaxSize = bucket->layout.iconSize;
    bucket->textMaxSize = bucket->layout.textSize;
    bucket->layout.iconSize.calculate(CalculationParameters(p.z + 1));
    bucket->layout.textSize.calculate(CalculationParameters(p.z + 1));

    bucket->parseFeatures(parameters.layer, filter);

    if (bucket->needsDependencies(parameters.glyphStore, parameters.spriteStore)) {
        parameters.partialParse = true;
    }

    // We do not add features if the parser is in a "partial" state because
    // the layer ordering needs to be respected when calculating text
    // collisions. Although, at this point, we requested all the resources
    // needed by this tile.
    if (!parameters.partialParse) {
        bucket->addFeatures(parameters.tileUID,
                            *spriteAtlas,
                            parameters.glyphAtlas,
                            parameters.glyphStore);
    }

    return std::move(bucket);
}

} // namespace style
} // namespace mbgl
