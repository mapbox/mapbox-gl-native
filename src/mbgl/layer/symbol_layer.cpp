#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> SymbolLayer::clone() const {
    std::unique_ptr<SymbolLayer> result = std::make_unique<SymbolLayer>();
    result->copy(*this);
    result->layout = layout;
    result->paints.paints = paints.paints;
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
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("icon-opacity", PropertyKey::IconOpacity, paint, value);
        parseProperty<PropertyTransition>("icon-opacity-transition", PropertyKey::IconOpacity, paint, value);
        parseProperty<Function<Color>>("icon-color", PropertyKey::IconColor, paint, value);
        parseProperty<PropertyTransition>("icon-color-transition", PropertyKey::IconColor, paint, value);
        parseProperty<Function<Color>>("icon-halo-color", PropertyKey::IconHaloColor, paint, value);
        parseProperty<PropertyTransition>("icon-halo-color-transition", PropertyKey::IconHaloColor, paint, value);
        parseProperty<Function<float>>("icon-halo-width", PropertyKey::IconHaloWidth, paint, value);
        parseProperty<PropertyTransition>("icon-halo-width-transition", PropertyKey::IconHaloWidth, paint, value);
        parseProperty<Function<float>>("icon-halo-blur", PropertyKey::IconHaloBlur, paint, value);
        parseProperty<PropertyTransition>("icon-halo-blur-transition", PropertyKey::IconHaloBlur, paint, value);
        parseProperty<Function<std::array<float, 2>>>("icon-translate", PropertyKey::IconTranslate, paint, value);
        parseProperty<PropertyTransition>("icon-translate-transition", PropertyKey::IconTranslate, paint, value);
        parseProperty<Function<TranslateAnchorType>>("icon-translate-anchor", PropertyKey::IconTranslateAnchor, paint, value);

        parseProperty<Function<float>>("text-opacity", PropertyKey::TextOpacity, paint, value);
        parseProperty<PropertyTransition>("text-opacity-transition", PropertyKey::TextOpacity, paint, value);
        parseProperty<Function<Color>>("text-color", PropertyKey::TextColor, paint, value);
        parseProperty<PropertyTransition>("text-color-transition", PropertyKey::TextColor, paint, value);
        parseProperty<Function<Color>>("text-halo-color", PropertyKey::TextHaloColor, paint, value);
        parseProperty<PropertyTransition>("text-halo-color-transition", PropertyKey::TextHaloColor, paint, value);
        parseProperty<Function<float>>("text-halo-width", PropertyKey::TextHaloWidth, paint, value);
        parseProperty<PropertyTransition>("text-halo-width-transition", PropertyKey::TextHaloWidth, paint, value);
        parseProperty<Function<float>>("text-halo-blur", PropertyKey::TextHaloBlur, paint, value);
        parseProperty<PropertyTransition>("text-halo-blur-transition", PropertyKey::TextHaloBlur, paint, value);
        parseProperty<Function<std::array<float, 2>>>("text-translate", PropertyKey::TextTranslate, paint, value);
        parseProperty<PropertyTransition>("text-translate-transition", PropertyKey::TextTranslate, paint, value);
        parseProperty<Function<TranslateAnchorType>>("text-translate-anchor", PropertyKey::TextTranslateAnchor, paint, value);
    });
}

void SymbolLayer::cascade(const StyleCascadeParameters& parameters) {
    paints.cascade(parameters);
}

bool SymbolLayer::hasTransitions() const {
    return paints.hasTransitions();
}

void SymbolLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::IconOpacity, properties.icon.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::IconColor, properties.icon.color, parameters);
    paints.calculateTransitioned(PropertyKey::IconHaloColor, properties.icon.halo_color, parameters);
    paints.calculateTransitioned(PropertyKey::IconHaloWidth, properties.icon.halo_width, parameters);
    paints.calculateTransitioned(PropertyKey::IconHaloBlur, properties.icon.halo_blur, parameters);
    paints.calculateTransitioned(PropertyKey::IconTranslate, properties.icon.translate, parameters);
    paints.calculate(PropertyKey::IconTranslateAnchor, properties.icon.translate_anchor, parameters);

    paints.calculateTransitioned(PropertyKey::TextOpacity, properties.text.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::TextColor, properties.text.color, parameters);
    paints.calculateTransitioned(PropertyKey::TextHaloColor, properties.text.halo_color, parameters);
    paints.calculateTransitioned(PropertyKey::TextHaloWidth, properties.text.halo_width, parameters);
    paints.calculateTransitioned(PropertyKey::TextHaloBlur, properties.text.halo_blur, parameters);
    paints.calculateTransitioned(PropertyKey::TextTranslate, properties.text.translate, parameters);
    paints.calculate(PropertyKey::TextTranslateAnchor, properties.text.translate_anchor, parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    layout.icon.size.calculate(parameters);
    layout.text.size.calculate(parameters);
    properties.icon.size = layout.icon.size;
    properties.text.size = layout.text.size;

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
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
