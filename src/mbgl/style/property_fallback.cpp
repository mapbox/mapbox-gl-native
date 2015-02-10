#include <mbgl/style/property_fallback.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/style_bucket.hpp>

namespace mbgl {

const std::map<PropertyKey, PropertyValue> PropertyFallbackValue::properties = {
    { PropertyKey::FillAntialias, defaultStyleProperties<FillProperties>().antialias },
    { PropertyKey::FillOpacity, defaultStyleProperties<FillProperties>().opacity },
    { PropertyKey::FillColor, defaultStyleProperties<FillProperties>().fill_color },
    // no FillOutlineColor on purpose.
    { PropertyKey::FillTranslate, defaultStyleProperties<FillProperties>().translate },
    { PropertyKey::FillTranslateAnchor, defaultStyleProperties<FillProperties>().translateAnchor },

    { PropertyKey::LineOpacity, defaultStyleProperties<LineProperties>().opacity },
    { PropertyKey::LineColor, defaultStyleProperties<LineProperties>().color },
    { PropertyKey::LineTranslate, defaultStyleProperties<LineProperties>().translate },
    { PropertyKey::LineTranslateAnchor, defaultStyleProperties<LineProperties>().translateAnchor },
    { PropertyKey::LineWidth, defaultStyleProperties<LineProperties>().width },
    { PropertyKey::LineGapWidth, defaultStyleProperties<LineProperties>().gap_width },
    { PropertyKey::LineBlur, defaultStyleProperties<LineProperties>().blur },

    { PropertyKey::IconOpacity, defaultStyleProperties<SymbolProperties>().icon.opacity },
    { PropertyKey::IconRotate, defaultStyleProperties<SymbolProperties>().icon.rotate },
    { PropertyKey::IconSize, defaultStyleProperties<SymbolProperties>().icon.size },
    { PropertyKey::IconColor, defaultStyleProperties<SymbolProperties>().icon.color },
    { PropertyKey::IconHaloColor, defaultStyleProperties<SymbolProperties>().icon.halo_color },
    { PropertyKey::IconHaloWidth, defaultStyleProperties<SymbolProperties>().icon.halo_width },
    { PropertyKey::IconHaloBlur, defaultStyleProperties<SymbolProperties>().icon.halo_blur },
    { PropertyKey::IconTranslate, defaultStyleProperties<SymbolProperties>().icon.translate },
    { PropertyKey::IconTranslateAnchor, defaultStyleProperties<SymbolProperties>().icon.translate_anchor },

    { PropertyKey::TextOpacity, defaultStyleProperties<SymbolProperties>().text.opacity },
    { PropertyKey::TextSize, defaultStyleProperties<SymbolProperties>().text.size },
    { PropertyKey::TextColor, defaultStyleProperties<SymbolProperties>().text.color },
    { PropertyKey::TextHaloColor, defaultStyleProperties<SymbolProperties>().text.halo_color },
    { PropertyKey::TextHaloWidth, defaultStyleProperties<SymbolProperties>().text.halo_width },
    { PropertyKey::TextHaloBlur, defaultStyleProperties<SymbolProperties>().text.halo_blur },
    { PropertyKey::TextTranslate, defaultStyleProperties<SymbolProperties>().text.translate },
    { PropertyKey::TextTranslateAnchor, defaultStyleProperties<SymbolProperties>().text.translate_anchor },

    { PropertyKey::RasterOpacity, defaultStyleProperties<RasterProperties>().opacity },
    { PropertyKey::RasterHueRotate, defaultStyleProperties<RasterProperties>().hue_rotate },
    { PropertyKey::RasterBrightnessLow, defaultStyleProperties<RasterProperties>().brightness[0] },
    { PropertyKey::RasterBrightnessHigh, defaultStyleProperties<RasterProperties>().brightness[1] },
    { PropertyKey::RasterSaturation, defaultStyleProperties<RasterProperties>().saturation },
    { PropertyKey::RasterContrast, defaultStyleProperties<RasterProperties>().contrast },
    { PropertyKey::RasterFade, defaultStyleProperties<RasterProperties>().fade },

    { PropertyKey::BackgroundOpacity, defaultStyleProperties<BackgroundProperties>().opacity },
    { PropertyKey::BackgroundColor, defaultStyleProperties<BackgroundProperties>().color },

    { PropertyKey::LineCap, defaultLayoutProperties<StyleBucketLine>().cap },
    { PropertyKey::LineJoin, defaultLayoutProperties<StyleBucketLine>().join },
    { PropertyKey::LineMiterLimit, defaultLayoutProperties<StyleBucketLine>().miter_limit },
    { PropertyKey::LineRoundLimit, defaultLayoutProperties<StyleBucketLine>().round_limit },

    { PropertyKey::SymbolPlacement, defaultLayoutProperties<StyleBucketSymbol>().placement },
    { PropertyKey::SymbolMinDistance, defaultLayoutProperties<StyleBucketSymbol>().min_distance },
    { PropertyKey::SymbolAvoidEdges, defaultLayoutProperties<StyleBucketSymbol>().avoid_edges },

    { PropertyKey::IconAllowOverlap, defaultLayoutProperties<StyleBucketSymbol>().icon.allow_overlap },
    { PropertyKey::IconIgnorePlacement, defaultLayoutProperties<StyleBucketSymbol>().icon.ignore_placement },
    { PropertyKey::IconOptional, defaultLayoutProperties<StyleBucketSymbol>().icon.optional },
    { PropertyKey::IconRotationAlignment, defaultLayoutProperties<StyleBucketSymbol>().icon.rotation_alignment },
    { PropertyKey::IconMaxSize, defaultLayoutProperties<StyleBucketSymbol>().icon.max_size },
    { PropertyKey::IconImage, defaultLayoutProperties<StyleBucketSymbol>().icon.image },
    { PropertyKey::IconRotate, defaultLayoutProperties<StyleBucketSymbol>().icon.rotate },
    { PropertyKey::IconPadding, defaultLayoutProperties<StyleBucketSymbol>().icon.padding },
    { PropertyKey::IconKeepUpright, defaultLayoutProperties<StyleBucketSymbol>().icon.keep_upright },
    { PropertyKey::IconOffset, defaultLayoutProperties<StyleBucketSymbol>().icon.offset },

    { PropertyKey::TextRotationAlignment, defaultLayoutProperties<StyleBucketSymbol>().text.rotation_alignment },
    { PropertyKey::TextField, defaultLayoutProperties<StyleBucketSymbol>().text.field },
    { PropertyKey::TextFont, defaultLayoutProperties<StyleBucketSymbol>().text.font },
    { PropertyKey::TextMaxSize, defaultLayoutProperties<StyleBucketSymbol>().text.max_size },
    { PropertyKey::TextMaxWidth, defaultLayoutProperties<StyleBucketSymbol>().text.max_width },
    { PropertyKey::TextLineHeight, defaultLayoutProperties<StyleBucketSymbol>().text.line_height },
    { PropertyKey::TextLetterSpacing, defaultLayoutProperties<StyleBucketSymbol>().text.letter_spacing },
    { PropertyKey::TextJustify, defaultLayoutProperties<StyleBucketSymbol>().text.justify },
    { PropertyKey::TextAnchor, defaultLayoutProperties<StyleBucketSymbol>().text.anchor },
    { PropertyKey::TextMaxAngle, defaultLayoutProperties<StyleBucketSymbol>().text.max_angle },
    { PropertyKey::TextRotate, defaultLayoutProperties<StyleBucketSymbol>().text.rotate },
    { PropertyKey::TextPadding, defaultLayoutProperties<StyleBucketSymbol>().text.padding },
    { PropertyKey::TextKeepUpright, defaultLayoutProperties<StyleBucketSymbol>().text.keep_upright },
    { PropertyKey::TextTransform, defaultLayoutProperties<StyleBucketSymbol>().text.transform },
    { PropertyKey::TextOffset, defaultLayoutProperties<StyleBucketSymbol>().text.offset },
    { PropertyKey::TextAllowOverlap, defaultLayoutProperties<StyleBucketSymbol>().text.allow_overlap },
    { PropertyKey::TextIgnorePlacement, defaultLayoutProperties<StyleBucketSymbol>().text.ignore_placement },
    { PropertyKey::TextOptional, defaultLayoutProperties<StyleBucketSymbol>().text.optional },

};

const PropertyValue PropertyFallbackValue::defaultProperty = false;

}
