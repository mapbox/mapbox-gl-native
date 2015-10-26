#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> SymbolLayer::clone() const {
    std::unique_ptr<SymbolLayer> result = std::make_unique<SymbolLayer>();
    result->copy(*this);
    result->layout = layout;
    result->paint = paint;
    return std::move(result);
}

void SymbolLayer::parseLayout(const JSVal& value) {
    layout.placement.parse("symbol-placement", value);
    layout.spacing.parse("symbol-spacing", value);
    layout.avoidEdges.parse("symbol-avoid-edges", value);

    layout.icon.allowOverlap.parse("icon-allow-overlap", value);
    layout.icon.ignorePlacement.parse("icon-ignore-placement", value);
    layout.icon.optional.parse("icon-optional", value);
    layout.icon.rotationAlignment.parse("icon-rotation-alignment", value);
    layout.icon.size.parse("icon-size", value);
    layout.icon.image.parse("icon-image", value);
    layout.icon.rotate.parse("icon-rotate", value);
    layout.icon.padding.parse("icon-padding", value);
    layout.icon.keepUpright.parse("icon-keep-upright", value);
    layout.icon.offset.parse("icon-offset", value);

    layout.text.rotationAlignment.parse("text-rotation-alignment", value);
    layout.text.field.parse("text-field", value);
    layout.text.font.parse("text-font", value);
    layout.text.size.parse("text-size", value);
    layout.text.maxWidth.parse("text-max-width", value);
    layout.text.lineHeight.parse("text-line-height", value);
    layout.text.letterSpacing.parse("text-letter-spacing", value);
    layout.text.justify.parse("text-justify", value);
    layout.text.anchor.parse("text-anchor", value);
    layout.text.maxAngle.parse("text-max-angle", value);
    layout.text.rotate.parse("text-rotate", value);
    layout.text.padding.parse("text-padding", value);
    layout.text.keepUpright.parse("text-keep-upright", value);
    layout.text.transform.parse("text-transform", value);
    layout.text.offset.parse("text-offset", value);
    layout.text.allowOverlap.parse("text-allow-overlap", value);
    layout.text.ignorePlacement.parse("text-ignore-placement", value);
    layout.text.optional.parse("text-optional", value);
}

void SymbolLayer::parsePaints(const JSVal& layer) {
    paint.icon.opacity.parse("icon-opacity", layer);
    paint.icon.color.parse("icon-color", layer);
    paint.icon.haloColor.parse("icon-halo-color", layer);
    paint.icon.haloWidth.parse("icon-halo-width", layer);
    paint.icon.haloBlur.parse("icon-halo-blur", layer);
    paint.icon.translate.parse("icon-translate", layer);
    paint.icon.translateAnchor.parse("icon-translate-anchor", layer);

    paint.text.opacity.parse("text-opacity", layer);
    paint.text.color.parse("text-color", layer);
    paint.text.haloColor.parse("text-halo-color", layer);
    paint.text.haloWidth.parse("text-halo-width", layer);
    paint.text.haloBlur.parse("text-halo-blur", layer);
    paint.text.translate.parse("text-translate", layer);
    paint.text.translateAnchor.parse("text-translate-anchor", layer);
}

void SymbolLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.icon.opacity.cascade(parameters);
    paint.icon.color.cascade(parameters);
    paint.icon.haloColor.cascade(parameters);
    paint.icon.haloWidth.cascade(parameters);
    paint.icon.haloBlur.cascade(parameters);
    paint.icon.translate.cascade(parameters);
    paint.icon.translateAnchor.cascade(parameters);

    paint.text.opacity.cascade(parameters);
    paint.text.color.cascade(parameters);
    paint.text.haloColor.cascade(parameters);
    paint.text.haloWidth.cascade(parameters);
    paint.text.haloBlur.cascade(parameters);
    paint.text.translate.cascade(parameters);
    paint.text.translateAnchor.cascade(parameters);
}

bool SymbolLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.icon.opacity.calculate(parameters);
    hasTransitions |= paint.icon.color.calculate(parameters);
    hasTransitions |= paint.icon.haloColor.calculate(parameters);
    hasTransitions |= paint.icon.haloWidth.calculate(parameters);
    hasTransitions |= paint.icon.haloBlur.calculate(parameters);
    hasTransitions |= paint.icon.translate.calculate(parameters);
    hasTransitions |= paint.icon.translateAnchor.calculate(parameters);

    hasTransitions |= paint.text.opacity.calculate(parameters);
    hasTransitions |= paint.text.color.calculate(parameters);
    hasTransitions |= paint.text.haloColor.calculate(parameters);
    hasTransitions |= paint.text.haloWidth.calculate(parameters);
    hasTransitions |= paint.text.haloBlur.calculate(parameters);
    hasTransitions |= paint.text.translate.calculate(parameters);
    hasTransitions |= paint.text.translateAnchor.calculate(parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    layout.icon.size.calculate(parameters);
    layout.text.size.calculate(parameters);
    paint.icon.size = layout.icon.size;
    paint.text.size = layout.text.size;

    passes = (paint.icon.isVisible() || paint.text.isVisible())
        ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> SymbolLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<SymbolBucket>(parameters.tileID.overscaling,
                                                 parameters.tileID.z);

    bucket->layout = layout;

    StyleCalculationParameters p(parameters.tileID.z);
    bucket->layout.placement.calculate(p);
    if (bucket->layout.placement.value == PlacementType::Line) {
        bucket->layout.icon.rotationAlignment = RotationAlignmentType::Map;
        bucket->layout.text.rotationAlignment = RotationAlignmentType::Map;
    };
    bucket->layout.spacing.calculate(p);
    bucket->layout.avoidEdges.calculate(p);

    bucket->layout.icon.allowOverlap.calculate(p);
    bucket->layout.icon.ignorePlacement.calculate(p);
    bucket->layout.icon.optional.calculate(p);
    bucket->layout.icon.rotationAlignment.calculate(p);
    bucket->layout.icon.image.calculate(p);
    bucket->layout.icon.padding.calculate(p);
    bucket->layout.icon.rotate.calculate(p);
    bucket->layout.icon.keepUpright.calculate(p);
    bucket->layout.icon.offset.calculate(p);

    bucket->layout.text.rotationAlignment.calculate(p);
    bucket->layout.text.field.calculate(p);
    bucket->layout.text.font.calculate(p);
    bucket->layout.text.maxWidth.calculate(p);
    bucket->layout.text.lineHeight.calculate(p);
    bucket->layout.text.letterSpacing.calculate(p);
    bucket->layout.text.maxAngle.calculate(p);
    bucket->layout.text.rotate.calculate(p);
    bucket->layout.text.padding.calculate(p);
    bucket->layout.text.ignorePlacement.calculate(p);
    bucket->layout.text.optional.calculate(p);
    bucket->layout.text.justify.calculate(p);
    bucket->layout.text.anchor.calculate(p);
    bucket->layout.text.keepUpright.calculate(p);
    bucket->layout.text.transform.calculate(p);
    bucket->layout.text.offset.calculate(p);
    bucket->layout.text.allowOverlap.calculate(p);

    bucket->layout.icon.size.calculate(StyleCalculationParameters(18));
    bucket->layout.text.size.calculate(StyleCalculationParameters(18));
    bucket->layout.iconMaxSize = bucket->layout.icon.size;
    bucket->layout.textMaxSize = bucket->layout.text.size;
    bucket->layout.icon.size.calculate(StyleCalculationParameters(p.z + 1));
    bucket->layout.text.size.calculate(StyleCalculationParameters(p.z + 1));

    bucket->parseFeatures(parameters.layer, filter);

    if (bucket->needsDependencies(parameters.glyphStore, parameters.sprite)) {
        parameters.partialParse = true;
    }

    // We do not add features if the parser is in a "partial" state because
    // the layer ordering needs to be respected when calculating text
    // collisions. Although, at this point, we requested all the resources
    // needed by this tile.
    if (!parameters.partialParse) {
        bucket->addFeatures(parameters.tileUID,
                            parameters.spriteAtlas,
                            parameters.glyphAtlas,
                            parameters.glyphStore,
                            parameters.collisionTile);
    }

    return std::move(bucket);
}

}
