#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> SymbolLayer::clone() const {
    return std::make_unique<SymbolLayer>(*this);
}

void SymbolLayer::parseLayout(const JSValue& value) {
    layout.symbolPlacement.parse("symbol-placement", value);
    layout.symbolSpacing.parse("symbol-spacing", value);
    layout.symbolAvoidEdges.parse("symbol-avoid-edges", value);

    layout.iconAllowOverlap.parse("icon-allow-overlap", value);
    layout.iconIgnorePlacement.parse("icon-ignore-placement", value);
    layout.iconOptional.parse("icon-optional", value);
    layout.iconRotationAlignment.parse("icon-rotation-alignment", value);
    layout.iconSize.parse("icon-size", value);
    layout.iconImage.parse("icon-image", value);
    layout.iconRotate.parse("icon-rotate", value);
    layout.iconPadding.parse("icon-padding", value);
    layout.iconKeepUpright.parse("icon-keep-upright", value);
    layout.iconOffset.parse("icon-offset", value);

    layout.textRotationAlignment.parse("text-rotation-alignment", value);
    layout.textField.parse("text-field", value);
    layout.textFont.parse("text-font", value);
    layout.textSize.parse("text-size", value);
    layout.textMaxWidth.parse("text-max-width", value);
    layout.textLineHeight.parse("text-line-height", value);
    layout.textLetterSpacing.parse("text-letter-spacing", value);
    layout.textJustify.parse("text-justify", value);
    layout.textAnchor.parse("text-anchor", value);
    layout.textMaxAngle.parse("text-max-angle", value);
    layout.textRotate.parse("text-rotate", value);
    layout.textPadding.parse("text-padding", value);
    layout.textKeepUpright.parse("text-keep-upright", value);
    layout.textTransform.parse("text-transform", value);
    layout.textOffset.parse("text-offset", value);
    layout.textAllowOverlap.parse("text-allow-overlap", value);
    layout.textIgnorePlacement.parse("text-ignore-placement", value);
    layout.textOptional.parse("text-optional", value);
}

void SymbolLayer::parsePaints(const JSValue& layer) {
    paint.iconOpacity.parse("icon-opacity", layer);
    paint.iconColor.parse("icon-color", layer);
    paint.iconHaloColor.parse("icon-halo-color", layer);
    paint.iconHaloWidth.parse("icon-halo-width", layer);
    paint.iconHaloBlur.parse("icon-halo-blur", layer);
    paint.iconTranslate.parse("icon-translate", layer);
    paint.iconTranslateAnchor.parse("icon-translate-anchor", layer);

    paint.textOpacity.parse("text-opacity", layer);
    paint.textColor.parse("text-color", layer);
    paint.textHaloColor.parse("text-halo-color", layer);
    paint.textHaloWidth.parse("text-halo-width", layer);
    paint.textHaloBlur.parse("text-halo-blur", layer);
    paint.textTranslate.parse("text-translate", layer);
    paint.textTranslateAnchor.parse("text-translate-anchor", layer);
}

void SymbolLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.iconOpacity.cascade(parameters);
    paint.iconColor.cascade(parameters);
    paint.iconHaloColor.cascade(parameters);
    paint.iconHaloWidth.cascade(parameters);
    paint.iconHaloBlur.cascade(parameters);
    paint.iconTranslate.cascade(parameters);
    paint.iconTranslateAnchor.cascade(parameters);

    paint.textOpacity.cascade(parameters);
    paint.textColor.cascade(parameters);
    paint.textHaloColor.cascade(parameters);
    paint.textHaloWidth.cascade(parameters);
    paint.textHaloBlur.cascade(parameters);
    paint.textTranslate.cascade(parameters);
    paint.textTranslateAnchor.cascade(parameters);
}

bool SymbolLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.iconOpacity.calculate(parameters);
    hasTransitions |= paint.iconColor.calculate(parameters);
    hasTransitions |= paint.iconHaloColor.calculate(parameters);
    hasTransitions |= paint.iconHaloWidth.calculate(parameters);
    hasTransitions |= paint.iconHaloBlur.calculate(parameters);
    hasTransitions |= paint.iconTranslate.calculate(parameters);
    hasTransitions |= paint.iconTranslateAnchor.calculate(parameters);

    hasTransitions |= paint.textOpacity.calculate(parameters);
    hasTransitions |= paint.textColor.calculate(parameters);
    hasTransitions |= paint.textHaloColor.calculate(parameters);
    hasTransitions |= paint.textHaloWidth.calculate(parameters);
    hasTransitions |= paint.textHaloBlur.calculate(parameters);
    hasTransitions |= paint.textTranslate.calculate(parameters);
    hasTransitions |= paint.textTranslateAnchor.calculate(parameters);

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

std::unique_ptr<Bucket> SymbolLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<SymbolBucket>(parameters.tileID.overscaleFactor(),
                                                 parameters.tileID.z,
                                                 parameters.mode);

    bucket->layout = layout;

    StyleCalculationParameters p(parameters.tileID.z);
    bucket->layout.symbolPlacement.calculate(p);
    if (bucket->layout.symbolPlacement.value == SymbolPlacementType::Line) {
        bucket->layout.iconRotationAlignment.value = RotationAlignmentType::Map;
        bucket->layout.textRotationAlignment.value = RotationAlignmentType::Map;
    };
    bucket->layout.symbolSpacing.calculate(p);
    bucket->layout.symbolAvoidEdges.calculate(p);

    bucket->layout.iconAllowOverlap.calculate(p);
    bucket->layout.iconIgnorePlacement.calculate(p);
    bucket->layout.iconOptional.calculate(p);
    bucket->layout.iconRotationAlignment.calculate(p);
    bucket->layout.iconImage.calculate(p);
    bucket->layout.iconPadding.calculate(p);
    bucket->layout.iconRotate.calculate(p);
    bucket->layout.iconKeepUpright.calculate(p);
    bucket->layout.iconOffset.calculate(p);

    bucket->layout.textRotationAlignment.calculate(p);
    bucket->layout.textField.calculate(p);
    bucket->layout.textFont.calculate(p);
    bucket->layout.textMaxWidth.calculate(p);
    bucket->layout.textLineHeight.calculate(p);
    bucket->layout.textLetterSpacing.calculate(p);
    bucket->layout.textMaxAngle.calculate(p);
    bucket->layout.textRotate.calculate(p);
    bucket->layout.textPadding.calculate(p);
    bucket->layout.textIgnorePlacement.calculate(p);
    bucket->layout.textOptional.calculate(p);
    bucket->layout.textJustify.calculate(p);
    bucket->layout.textAnchor.calculate(p);
    bucket->layout.textKeepUpright.calculate(p);
    bucket->layout.textTransform.calculate(p);
    bucket->layout.textOffset.calculate(p);
    bucket->layout.textAllowOverlap.calculate(p);

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
