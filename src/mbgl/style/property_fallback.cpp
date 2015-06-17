#include <mbgl/style/property_fallback.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layout.hpp>

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

    { PropertyKey::CircleRadius, defaultStyleProperties<CircleProperties>().radius },
    { PropertyKey::CircleColor, defaultStyleProperties<CircleProperties>().color },
    { PropertyKey::CircleOpacity, defaultStyleProperties<CircleProperties>().opacity },
    { PropertyKey::CircleTranslate, defaultStyleProperties<CircleProperties>().translate },
    { PropertyKey::CircleTranslateAnchor, defaultStyleProperties<CircleProperties>().translateAnchor },
    { PropertyKey::CircleBlur, defaultStyleProperties<CircleProperties>().blur },

    { PropertyKey::IconOpacity, defaultStyleProperties<SymbolProperties>().icon.opacity },
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

    { PropertyKey::LineCap, defaultStyleLayout<StyleLayoutLine>().cap },
    { PropertyKey::LineJoin, defaultStyleLayout<StyleLayoutLine>().join },
    { PropertyKey::LineMiterLimit, defaultStyleLayout<StyleLayoutLine>().miter_limit },
    { PropertyKey::LineRoundLimit, defaultStyleLayout<StyleLayoutLine>().round_limit },

    { PropertyKey::SymbolPlacement, defaultStyleLayout<StyleLayoutSymbol>().placement },
    { PropertyKey::SymbolSpacing, defaultStyleLayout<StyleLayoutSymbol>().spacing },
    { PropertyKey::SymbolAvoidEdges, defaultStyleLayout<StyleLayoutSymbol>().avoid_edges },

    { PropertyKey::IconAllowOverlap, defaultStyleLayout<StyleLayoutSymbol>().icon.allow_overlap },
    { PropertyKey::IconIgnorePlacement, defaultStyleLayout<StyleLayoutSymbol>().icon.ignore_placement },
    { PropertyKey::IconOptional, defaultStyleLayout<StyleLayoutSymbol>().icon.optional },
    { PropertyKey::IconRotationAlignment, defaultStyleLayout<StyleLayoutSymbol>().icon.rotation_alignment },
    { PropertyKey::IconMaxSize, defaultStyleLayout<StyleLayoutSymbol>().icon.max_size },
    { PropertyKey::IconImage, defaultStyleLayout<StyleLayoutSymbol>().icon.image },
    { PropertyKey::IconRotate, defaultStyleLayout<StyleLayoutSymbol>().icon.rotate },
    { PropertyKey::IconPadding, defaultStyleLayout<StyleLayoutSymbol>().icon.padding },
    { PropertyKey::IconKeepUpright, defaultStyleLayout<StyleLayoutSymbol>().icon.keep_upright },
    { PropertyKey::IconOffset, defaultStyleLayout<StyleLayoutSymbol>().icon.offset },

    { PropertyKey::TextRotationAlignment, defaultStyleLayout<StyleLayoutSymbol>().text.rotation_alignment },
    { PropertyKey::TextField, defaultStyleLayout<StyleLayoutSymbol>().text.field },
    { PropertyKey::TextFont, defaultStyleLayout<StyleLayoutSymbol>().text.font },
    { PropertyKey::TextMaxSize, defaultStyleLayout<StyleLayoutSymbol>().text.max_size },
    { PropertyKey::TextMaxWidth, defaultStyleLayout<StyleLayoutSymbol>().text.max_width },
    { PropertyKey::TextLineHeight, defaultStyleLayout<StyleLayoutSymbol>().text.line_height },
    { PropertyKey::TextLetterSpacing, defaultStyleLayout<StyleLayoutSymbol>().text.letter_spacing },
    { PropertyKey::TextJustify, defaultStyleLayout<StyleLayoutSymbol>().text.justify },
    { PropertyKey::TextAnchor, defaultStyleLayout<StyleLayoutSymbol>().text.anchor },
    { PropertyKey::TextMaxAngle, defaultStyleLayout<StyleLayoutSymbol>().text.max_angle },
    { PropertyKey::TextRotate, defaultStyleLayout<StyleLayoutSymbol>().text.rotate },
    { PropertyKey::TextPadding, defaultStyleLayout<StyleLayoutSymbol>().text.padding },
    { PropertyKey::TextKeepUpright, defaultStyleLayout<StyleLayoutSymbol>().text.keep_upright },
    { PropertyKey::TextTransform, defaultStyleLayout<StyleLayoutSymbol>().text.transform },
    { PropertyKey::TextOffset, defaultStyleLayout<StyleLayoutSymbol>().text.offset },
    { PropertyKey::TextAllowOverlap, defaultStyleLayout<StyleLayoutSymbol>().text.allow_overlap },
    { PropertyKey::TextIgnorePlacement, defaultStyleLayout<StyleLayoutSymbol>().text.ignore_placement },
    { PropertyKey::TextOptional, defaultStyleLayout<StyleLayoutSymbol>().text.optional },

};

const PropertyValue PropertyFallbackValue::defaultProperty = false;

}
