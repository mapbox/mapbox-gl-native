#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>

namespace mbgl {

void SymbolLayer::parseLayout(const JSVal& value) {
    parseProperty<Function<PlacementType>>("symbol-placement", PropertyKey::SymbolPlacement, layout, value);
    parseProperty<Function<float>>("symbol-spacing", PropertyKey::SymbolSpacing, layout, value);
    parseProperty<Function<bool>>("symbol-avoid-edges", PropertyKey::SymbolAvoidEdges, layout, value);
    parseProperty<Function<bool>>("icon-allow-overlap", PropertyKey::IconAllowOverlap, layout, value);
    parseProperty<Function<bool>>("icon-ignore-placement", PropertyKey::IconIgnorePlacement, layout, value);
    parseProperty<Function<bool>>("icon-optional", PropertyKey::IconOptional, layout, value);
    parseProperty<Function<RotationAlignmentType>>("icon-rotation-alignment", PropertyKey::IconRotationAlignment, layout, value);
    parseProperty<Function<float>>("icon-size", PropertyKey::IconSize, layout, value);
    parseProperty<Function<std::string>>("icon-image", PropertyKey::IconImage, layout, value);
    parseProperty<Function<float>>("icon-rotate", PropertyKey::IconRotate, layout, value);
    parseProperty<Function<float>>("icon-padding", PropertyKey::IconPadding, layout, value);
    parseProperty<Function<bool>>("icon-keep-upright", PropertyKey::IconKeepUpright, layout, value);
    parseProperty<Function<std::array<float, 2>>>("icon-offset", PropertyKey::IconOffset, layout, value);
    parseProperty<Function<RotationAlignmentType>>("text-rotation-alignment", PropertyKey::TextRotationAlignment, layout, value);
    parseProperty<Function<std::string>>("text-field", PropertyKey::TextField, layout, value);
    parseProperty<Function<std::string>>("text-font", PropertyKey::TextFont, layout, value);
    parseProperty<Function<float>>("text-size", PropertyKey::TextSize, layout, value);
    parseProperty<Function<float>>("text-max-width", PropertyKey::TextMaxWidth, layout, value);
    parseProperty<Function<float>>("text-line-height", PropertyKey::TextLineHeight, layout, value);
    parseProperty<Function<float>>("text-letter-spacing", PropertyKey::TextLetterSpacing, layout, value);
    parseProperty<Function<TextJustifyType>>("text-justify", PropertyKey::TextJustify, layout, value);
    parseProperty<Function<TextAnchorType>>("text-anchor", PropertyKey::TextAnchor, layout, value);
    parseProperty<Function<float>>("text-max-angle", PropertyKey::TextMaxAngle, layout, value);
    parseProperty<Function<float>>("text-rotate", PropertyKey::TextRotate, layout, value);
    parseProperty<Function<float>>("text-padding", PropertyKey::TextPadding, layout, value);
    parseProperty<Function<bool>>("text-keep-upright", PropertyKey::TextKeepUpright, layout, value);
    parseProperty<Function<TextTransformType>>("text-transform", PropertyKey::TextTransform, layout, value);
    parseProperty<Function<std::array<float, 2>>>("text-offset", PropertyKey::TextOffset, layout, value);
    parseProperty<Function<bool>>("text-allow-overlap", PropertyKey::TextAllowOverlap, layout, value);
    parseProperty<Function<bool>>("text-ignore-placement", PropertyKey::TextIgnorePlacement, layout, value);
    parseProperty<Function<bool>>("text-optional", PropertyKey::TextOptional, layout, value);
}

void SymbolLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("icon-opacity", PropertyKey::IconOpacity, paint, value);
        parseProperty<PropertyTransition>("icon-opacity-transition", PropertyKey::IconOpacity, paint, value);
        parseProperty<Function<float>>("icon-size", PropertyKey::IconSize, paint, value);
        parseProperty<PropertyTransition>("icon-size-transition", PropertyKey::IconSize, paint, value);
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
        parseProperty<Function<float>>("text-size", PropertyKey::TextSize, paint, value);
        parseProperty<PropertyTransition>("text-size-transition", PropertyKey::TextSize, paint, value);
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
    auto it = layout.properties.find(PropertyKey::IconSize);
    if (it != layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(parameters);
        properties.icon.size = mapbox::util::apply_visitor(evaluator, it->second);
    }
    it = layout.properties.find(PropertyKey::TextSize);
    if (it != layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(parameters);
        properties.text.size = mapbox::util::apply_visitor(evaluator, it->second);
    }

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

std::unique_ptr<Bucket> SymbolLayer::createBucket(StyleBucketParameters& parameters) const {
    const float z = parameters.tileID.z;
    auto bucket = std::make_unique<SymbolBucket>(parameters.tileID.overscaling, z);

    layout.calculate(PropertyKey::SymbolPlacement, bucket->layout.placement, z);
    if (bucket->layout.placement == PlacementType::Line) {
        bucket->layout.icon.rotation_alignment = RotationAlignmentType::Map;
        bucket->layout.text.rotation_alignment = RotationAlignmentType::Map;
    };
    layout.calculate(PropertyKey::SymbolSpacing, bucket->layout.spacing, z);
    layout.calculate(PropertyKey::SymbolAvoidEdges, bucket->layout.avoid_edges, z);

    layout.calculate(PropertyKey::IconAllowOverlap, bucket->layout.icon.allow_overlap, z);
    layout.calculate(PropertyKey::IconIgnorePlacement, bucket->layout.icon.ignore_placement, z);
    layout.calculate(PropertyKey::IconOptional, bucket->layout.icon.optional, z);
    layout.calculate(PropertyKey::IconRotationAlignment, bucket->layout.icon.rotation_alignment, z);
    layout.calculate(PropertyKey::IconImage, bucket->layout.icon.image, z);
    layout.calculate(PropertyKey::IconPadding, bucket->layout.icon.padding, z);
    layout.calculate(PropertyKey::IconRotate, bucket->layout.icon.rotate, z);
    layout.calculate(PropertyKey::IconKeepUpright, bucket->layout.icon.keep_upright, z);
    layout.calculate(PropertyKey::IconOffset, bucket->layout.icon.offset, z);

    layout.calculate(PropertyKey::TextRotationAlignment, bucket->layout.text.rotation_alignment, z);
    layout.calculate(PropertyKey::TextField, bucket->layout.text.field, z);
    layout.calculate(PropertyKey::TextFont, bucket->layout.text.font, z);
    layout.calculate(PropertyKey::TextMaxWidth, bucket->layout.text.max_width, z);
    layout.calculate(PropertyKey::TextLineHeight, bucket->layout.text.line_height, z);
    layout.calculate(PropertyKey::TextLetterSpacing, bucket->layout.text.letter_spacing, z);
    layout.calculate(PropertyKey::TextMaxAngle, bucket->layout.text.max_angle, z);
    layout.calculate(PropertyKey::TextRotate, bucket->layout.text.rotate, z);
    layout.calculate(PropertyKey::TextPadding, bucket->layout.text.padding, z);
    layout.calculate(PropertyKey::TextIgnorePlacement, bucket->layout.text.ignore_placement, z);
    layout.calculate(PropertyKey::TextOptional, bucket->layout.text.optional, z);
    layout.calculate(PropertyKey::TextJustify, bucket->layout.text.justify, z);
    layout.calculate(PropertyKey::TextAnchor, bucket->layout.text.anchor, z);
    layout.calculate(PropertyKey::TextKeepUpright, bucket->layout.text.keep_upright, z);
    layout.calculate(PropertyKey::TextTransform, bucket->layout.text.transform, z);
    layout.calculate(PropertyKey::TextOffset, bucket->layout.text.offset, z);
    layout.calculate(PropertyKey::TextAllowOverlap, bucket->layout.text.allow_overlap, z);

    layout.calculate(PropertyKey::IconSize, bucket->layout.icon.size, z + 1);
    layout.calculate(PropertyKey::IconSize, bucket->layout.icon.max_size, 18);
    layout.calculate(PropertyKey::TextSize, bucket->layout.text.size, z + 1);
    layout.calculate(PropertyKey::TextSize, bucket->layout.text.max_size, 18);

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
