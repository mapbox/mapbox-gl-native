#include <mbgl/layer/symbol_layer_impl.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>

namespace mbgl {

void SymbolLayer::Impl::parseLayout(const JSValue& value) {
    layout.parse(value);
}

void SymbolLayer::Impl::parsePaints(const JSValue& layer) {
    paint.parse(layer);
}

void SymbolLayer::Impl::cascade(const StyleCascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool SymbolLayer::Impl::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    layout.iconSize.calculate(parameters);
    layout.textSize.calculate(parameters);
    iconSize = layout.iconSize;
    textSize = layout.textSize;

    passes = ((paint.iconOpacity > 0 && (paint.iconColor.value[3] > 0 || paint.iconHaloColor.value[3] > 0) && iconSize > 0)
           || (paint.textOpacity > 0 && (paint.textColor.value[3] > 0 || paint.textHaloColor.value[3] > 0) && textSize > 0))
        ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> SymbolLayer::Impl::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<SymbolBucket>(parameters.tileID.overscaleFactor(),
                                                 parameters.tileID.overscaledZ,
                                                 parameters.mode,
                                                 id,
                                                 parameters.layer.getName());

    bucket->layout = layout;

    StyleCalculationParameters p(parameters.tileID.overscaledZ);
    bucket->layout.symbolPlacement.calculate(p);
    if (bucket->layout.symbolPlacement.value == SymbolPlacementType::Line) {
        bucket->layout.iconRotationAlignment.value = RotationAlignmentType::Map;
        bucket->layout.textRotationAlignment.value = RotationAlignmentType::Map;
    };

    bucket->layout.recalculate(p);

    bucket->layout.iconSize.calculate(StyleCalculationParameters(18));
    bucket->layout.textSize.calculate(StyleCalculationParameters(18));
    bucket->iconMaxSize = bucket->layout.iconSize;
    bucket->textMaxSize = bucket->layout.textSize;
    bucket->layout.iconSize.calculate(StyleCalculationParameters(p.z + 1));
    bucket->layout.textSize.calculate(StyleCalculationParameters(p.z + 1));

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

} // namespace mbgl
