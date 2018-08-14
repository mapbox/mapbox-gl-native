// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct SymbolPlacement : LayoutProperty<SymbolPlacementType> {
    static constexpr const char * key = "symbol-placement";
    static SymbolPlacementType defaultValue() { return SymbolPlacementType::Point; }
};

struct SymbolSpacing : LayoutProperty<float> {
    static constexpr const char * key = "symbol-spacing";
    static float defaultValue() { return 250; }
};

struct SymbolAvoidEdges : LayoutProperty<bool> {
    static constexpr const char * key = "symbol-avoid-edges";
    static bool defaultValue() { return false; }
};

struct IconAllowOverlap : LayoutProperty<bool> {
    static constexpr const char * key = "icon-allow-overlap";
    static bool defaultValue() { return false; }
};

struct IconIgnorePlacement : LayoutProperty<bool> {
    static constexpr const char * key = "icon-ignore-placement";
    static bool defaultValue() { return false; }
};

struct IconOptional : LayoutProperty<bool> {
    static constexpr const char * key = "icon-optional";
    static bool defaultValue() { return false; }
};

struct IconRotationAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "icon-rotation-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct IconSize : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "icon-size";
    static float defaultValue() { return 1; }
};

struct IconTextFit : LayoutProperty<IconTextFitType> {
    static constexpr const char * key = "icon-text-fit";
    static IconTextFitType defaultValue() { return IconTextFitType::None; }
};

struct IconTextFitPadding : LayoutProperty<std::array<float, 4>> {
    static constexpr const char * key = "icon-text-fit-padding";
    static std::array<float, 4> defaultValue() { return {{ 0, 0, 0, 0 }}; }
};

struct IconImage : DataDrivenLayoutProperty<std::string> {
    static constexpr const char * key = "icon-image";
    static std::string defaultValue() { return ""; }
};

struct IconRotate : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "icon-rotate";
    static float defaultValue() { return 0; }
};

struct IconPadding : LayoutProperty<float> {
    static constexpr const char * key = "icon-padding";
    static float defaultValue() { return 2; }
};

struct IconKeepUpright : LayoutProperty<bool> {
    static constexpr const char * key = "icon-keep-upright";
    static bool defaultValue() { return false; }
};

struct IconOffset : DataDrivenLayoutProperty<std::array<float, 2>> {
    static constexpr const char * key = "icon-offset";
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct IconAnchor : DataDrivenLayoutProperty<SymbolAnchorType> {
    static constexpr const char * key = "icon-anchor";
    static SymbolAnchorType defaultValue() { return SymbolAnchorType::Center; }
};

struct IconPitchAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "icon-pitch-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextPitchAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "text-pitch-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextRotationAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "text-rotation-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextField : DataDrivenLayoutProperty<std::string> {
    static constexpr const char * key = "text-field";
    static std::string defaultValue() { return ""; }
};

struct TextFont : DataDrivenLayoutProperty<std::vector<std::string>> {
    static constexpr const char * key = "text-font";
    static std::vector<std::string> defaultValue() { return { "Open Sans Regular", "Arial Unicode MS Regular" }; }
};

struct TextSize : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "text-size";
    static float defaultValue() { return 16; }
};

struct TextMaxWidth : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "text-max-width";
    static float defaultValue() { return 10; }
};

struct TextLineHeight : LayoutProperty<float> {
    static constexpr const char * key = "text-line-height";
    static float defaultValue() { return 1.2; }
};

struct TextLetterSpacing : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "text-letter-spacing";
    static float defaultValue() { return 0; }
};

struct TextJustify : DataDrivenLayoutProperty<TextJustifyType> {
    static constexpr const char * key = "text-justify";
    static TextJustifyType defaultValue() { return TextJustifyType::Center; }
};

struct TextAnchor : DataDrivenLayoutProperty<SymbolAnchorType> {
    static constexpr const char * key = "text-anchor";
    static SymbolAnchorType defaultValue() { return SymbolAnchorType::Center; }
};

struct TextMaxAngle : LayoutProperty<float> {
    static constexpr const char * key = "text-max-angle";
    static float defaultValue() { return 45; }
};

struct TextRotate : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "text-rotate";
    static float defaultValue() { return 0; }
};

struct TextPadding : LayoutProperty<float> {
    static constexpr const char * key = "text-padding";
    static float defaultValue() { return 2; }
};

struct TextKeepUpright : LayoutProperty<bool> {
    static constexpr const char * key = "text-keep-upright";
    static bool defaultValue() { return true; }
};

struct TextTransform : DataDrivenLayoutProperty<TextTransformType> {
    static constexpr const char * key = "text-transform";
    static TextTransformType defaultValue() { return TextTransformType::None; }
};

struct TextOffset : DataDrivenLayoutProperty<std::array<float, 2>> {
    static constexpr const char * key = "text-offset";
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextAllowOverlap : LayoutProperty<bool> {
    static constexpr const char * key = "text-allow-overlap";
    static bool defaultValue() { return false; }
};

struct TextIgnorePlacement : LayoutProperty<bool> {
    static constexpr const char * key = "text-ignore-placement";
    static bool defaultValue() { return false; }
};

struct TextOptional : LayoutProperty<bool> {
    static constexpr const char * key = "text-optional";
    static bool defaultValue() { return false; }
};

struct IconOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
    static float defaultValue() { return 1; }
};

struct IconColor : DataDrivenPaintProperty<Color, attributes::a_fill_color, uniforms::u_fill_color> {
    static Color defaultValue() { return Color::black(); }
};

struct IconHaloColor : DataDrivenPaintProperty<Color, attributes::a_halo_color, uniforms::u_halo_color> {
    static Color defaultValue() { return {}; }
};

struct IconHaloWidth : DataDrivenPaintProperty<float, attributes::a_halo_width, uniforms::u_halo_width> {
    static float defaultValue() { return 0; }
};

struct IconHaloBlur : DataDrivenPaintProperty<float, attributes::a_halo_blur, uniforms::u_halo_blur> {
    static float defaultValue() { return 0; }
};

struct IconTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct IconTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct TextOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
    static float defaultValue() { return 1; }
};

struct TextColor : DataDrivenPaintProperty<Color, attributes::a_fill_color, uniforms::u_fill_color> {
    static Color defaultValue() { return Color::black(); }
};

struct TextHaloColor : DataDrivenPaintProperty<Color, attributes::a_halo_color, uniforms::u_halo_color> {
    static Color defaultValue() { return {}; }
};

struct TextHaloWidth : DataDrivenPaintProperty<float, attributes::a_halo_width, uniforms::u_halo_width> {
    static float defaultValue() { return 0; }
};

struct TextHaloBlur : DataDrivenPaintProperty<float, attributes::a_halo_blur, uniforms::u_halo_blur> {
    static float defaultValue() { return 0; }
};

struct TextTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

class SymbolLayoutProperties {
public:
    class Evaluated {
    public:
        SymbolPlacementType symbolPlacement;
        float symbolSpacing;
        bool symbolAvoidEdges;
        bool iconAllowOverlap;
        bool iconIgnorePlacement;
        bool iconOptional;
        AlignmentType iconRotationAlignment;
        float iconSize;
        IconTextFitType iconTextFit;
        std::array<float, 4> iconTextFitPadding;
        std::string iconImage;
        float iconRotate;
        float iconPadding;
        bool iconKeepUpright;
        std::array<float, 2> iconOffset;
        SymbolAnchorType iconAnchor;
        AlignmentType iconPitchAlignment;
        AlignmentType textPitchAlignment;
        AlignmentType textRotationAlignment;
        std::string textField;
        std::vector<std::string> textFont;
        float textSize;
        float textMaxWidth;
        float textLineHeight;
        float textLetterSpacing;
        TextJustifyType textJustify;
        SymbolAnchorType textAnchor;
        float textMaxAngle;
        float textRotate;
        float textPadding;
        bool textKeepUpright;
        TextTransformType textTransform;
        std::array<float, 2> textOffset;
        bool textAllowOverlap;
        bool textIgnorePlacement;
        bool textOptional;
    };

    class PossiblyEvaluated {
    public:
        SymbolPlacementType symbolPlacement;
        float symbolSpacing;
        bool symbolAvoidEdges;
        bool iconAllowOverlap;
        bool iconIgnorePlacement;
        bool iconOptional;
        AlignmentType iconRotationAlignment;
        PossiblyEvaluatedPropertyValue<float> iconSize;
        IconTextFitType iconTextFit;
        std::array<float, 4> iconTextFitPadding;
        PossiblyEvaluatedPropertyValue<std::string> iconImage;
        PossiblyEvaluatedPropertyValue<float> iconRotate;
        float iconPadding;
        bool iconKeepUpright;
        PossiblyEvaluatedPropertyValue<std::array<float, 2>> iconOffset;
        PossiblyEvaluatedPropertyValue<SymbolAnchorType> iconAnchor;
        AlignmentType iconPitchAlignment;
        AlignmentType textPitchAlignment;
        AlignmentType textRotationAlignment;
        PossiblyEvaluatedPropertyValue<std::string> textField;
        PossiblyEvaluatedPropertyValue<std::vector<std::string>> textFont;
        PossiblyEvaluatedPropertyValue<float> textSize;
        PossiblyEvaluatedPropertyValue<float> textMaxWidth;
        float textLineHeight;
        PossiblyEvaluatedPropertyValue<float> textLetterSpacing;
        PossiblyEvaluatedPropertyValue<TextJustifyType> textJustify;
        PossiblyEvaluatedPropertyValue<SymbolAnchorType> textAnchor;
        float textMaxAngle;
        PossiblyEvaluatedPropertyValue<float> textRotate;
        float textPadding;
        bool textKeepUpright;
        PossiblyEvaluatedPropertyValue<TextTransformType> textTransform;
        PossiblyEvaluatedPropertyValue<std::array<float, 2>> textOffset;
        bool textAllowOverlap;
        bool textIgnorePlacement;
        bool textOptional;

        Evaluated evaluate(float z, const GeometryTileFeature& feature) const;
    };

    class Unevaluated {
    public:
        PropertyValue<SymbolPlacementType> symbolPlacement;
        PropertyValue<float> symbolSpacing;
        PropertyValue<bool> symbolAvoidEdges;
        PropertyValue<bool> iconAllowOverlap;
        PropertyValue<bool> iconIgnorePlacement;
        PropertyValue<bool> iconOptional;
        PropertyValue<AlignmentType> iconRotationAlignment;
        PropertyValue<float> iconSize;
        PropertyValue<IconTextFitType> iconTextFit;
        PropertyValue<std::array<float, 4>> iconTextFitPadding;
        PropertyValue<std::string> iconImage;
        PropertyValue<float> iconRotate;
        PropertyValue<float> iconPadding;
        PropertyValue<bool> iconKeepUpright;
        PropertyValue<std::array<float, 2>> iconOffset;
        PropertyValue<SymbolAnchorType> iconAnchor;
        PropertyValue<AlignmentType> iconPitchAlignment;
        PropertyValue<AlignmentType> textPitchAlignment;
        PropertyValue<AlignmentType> textRotationAlignment;
        PropertyValue<std::string> textField;
        PropertyValue<std::vector<std::string>> textFont;
        PropertyValue<float> textSize;
        PropertyValue<float> textMaxWidth;
        PropertyValue<float> textLineHeight;
        PropertyValue<float> textLetterSpacing;
        PropertyValue<TextJustifyType> textJustify;
        PropertyValue<SymbolAnchorType> textAnchor;
        PropertyValue<float> textMaxAngle;
        PropertyValue<float> textRotate;
        PropertyValue<float> textPadding;
        PropertyValue<bool> textKeepUpright;
        PropertyValue<TextTransformType> textTransform;
        PropertyValue<std::array<float, 2>> textOffset;
        PropertyValue<bool> textAllowOverlap;
        PropertyValue<bool> textIgnorePlacement;
        PropertyValue<bool> textOptional;

        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;

        friend bool operator==(const Unevaluated&, const Unevaluated&);
        friend inline bool operator!=(const Unevaluated& lhs, const Unevaluated& rhs) { return !(lhs == rhs); }
    };
};

class SymbolPaintProperties {
public:

    class PossiblyEvaluated {
    public:
        PossiblyEvaluatedPropertyValue<float> iconOpacity;
        PossiblyEvaluatedPropertyValue<Color> iconColor;
        PossiblyEvaluatedPropertyValue<Color> iconHaloColor;
        PossiblyEvaluatedPropertyValue<float> iconHaloWidth;
        PossiblyEvaluatedPropertyValue<float> iconHaloBlur;
        std::array<float, 2> iconTranslate;
        TranslateAnchorType iconTranslateAnchor;
        PossiblyEvaluatedPropertyValue<float> textOpacity;
        PossiblyEvaluatedPropertyValue<Color> textColor;
        PossiblyEvaluatedPropertyValue<Color> textHaloColor;
        PossiblyEvaluatedPropertyValue<float> textHaloWidth;
        PossiblyEvaluatedPropertyValue<float> textHaloBlur;
        std::array<float, 2> textTranslate;
        TranslateAnchorType textTranslateAnchor;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> iconOpacity;
        style::Transitioning<PropertyValue<Color>> iconColor;
        style::Transitioning<PropertyValue<Color>> iconHaloColor;
        style::Transitioning<PropertyValue<float>> iconHaloWidth;
        style::Transitioning<PropertyValue<float>> iconHaloBlur;
        style::Transitioning<PropertyValue<std::array<float, 2>>> iconTranslate;
        style::Transitioning<PropertyValue<TranslateAnchorType>> iconTranslateAnchor;
        style::Transitioning<PropertyValue<float>> textOpacity;
        style::Transitioning<PropertyValue<Color>> textColor;
        style::Transitioning<PropertyValue<Color>> textHaloColor;
        style::Transitioning<PropertyValue<float>> textHaloWidth;
        style::Transitioning<PropertyValue<float>> textHaloBlur;
        style::Transitioning<PropertyValue<std::array<float, 2>>> textTranslate;
        style::Transitioning<PropertyValue<TranslateAnchorType>> textTranslateAnchor;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> iconOpacity;
        style::Transitionable<PropertyValue<Color>> iconColor;
        style::Transitionable<PropertyValue<Color>> iconHaloColor;
        style::Transitionable<PropertyValue<float>> iconHaloWidth;
        style::Transitionable<PropertyValue<float>> iconHaloBlur;
        style::Transitionable<PropertyValue<std::array<float, 2>>> iconTranslate;
        style::Transitionable<PropertyValue<TranslateAnchorType>> iconTranslateAnchor;
        style::Transitionable<PropertyValue<float>> textOpacity;
        style::Transitionable<PropertyValue<Color>> textColor;
        style::Transitionable<PropertyValue<Color>> textHaloColor;
        style::Transitionable<PropertyValue<float>> textHaloWidth;
        style::Transitionable<PropertyValue<float>> textHaloBlur;
        style::Transitionable<PropertyValue<std::array<float, 2>>> textTranslate;
        style::Transitionable<PropertyValue<TranslateAnchorType>> textTranslateAnchor;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};

// {icon,text}-specific paint-property packs for use in the symbol Programs.
// Since each program deals either with icons or text, using a smaller property set
// lets us avoid unnecessarily binding attributes for properties the program wouldn't use.


class IconPaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_opacity::Type>> iconOpacity;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_fill_color::Type>> iconColor;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_halo_color::Type>> iconHaloColor;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_halo_width::Type>> iconHaloWidth;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_halo_blur::Type>> iconHaloBlur;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_opacity>,
            ZoomInterpolatedAttribute<attributes::a_fill_color>,
            ZoomInterpolatedAttribute<attributes::a_halo_color>,
            ZoomInterpolatedAttribute<attributes::a_halo_width>,
            ZoomInterpolatedAttribute<attributes::a_halo_blur>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_opacity>,
            InterpolationUniform<attributes::a_fill_color>,
            InterpolationUniform<attributes::a_halo_color>,
            InterpolationUniform<attributes::a_halo_width>,
            InterpolationUniform<attributes::a_halo_blur>,
            uniforms::u_opacity,
            uniforms::u_fill_color,
            uniforms::u_halo_color,
            uniforms::u_halo_width,
            uniforms::u_halo_blur
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        PossiblyEvaluatedPropertyValue<float> iconOpacity;
        PossiblyEvaluatedPropertyValue<Color> iconColor;
        PossiblyEvaluatedPropertyValue<Color> iconHaloColor;
        PossiblyEvaluatedPropertyValue<float> iconHaloWidth;
        PossiblyEvaluatedPropertyValue<float> iconHaloBlur;
        std::array<float, 2> iconTranslate;
        TranslateAnchorType iconTranslateAnchor;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };
};


class TextPaintProperties {
public:
    class PossiblyEvaluated;

    class Binders {
    public:
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_opacity::Type>> textOpacity;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_fill_color::Type>> textColor;
        std::unique_ptr<PaintPropertyBinder<Color, typename attributes::a_halo_color::Type>> textHaloColor;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_halo_width::Type>> textHaloWidth;
        std::unique_ptr<PaintPropertyBinder<float, typename attributes::a_halo_blur::Type>> textHaloBlur;

        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<
            ZoomInterpolatedAttribute<attributes::a_opacity>,
            ZoomInterpolatedAttribute<attributes::a_fill_color>,
            ZoomInterpolatedAttribute<attributes::a_halo_color>,
            ZoomInterpolatedAttribute<attributes::a_halo_width>,
            ZoomInterpolatedAttribute<attributes::a_halo_blur>
        >;

        using Uniforms = gl::Uniforms<
            InterpolationUniform<attributes::a_opacity>,
            InterpolationUniform<attributes::a_fill_color>,
            InterpolationUniform<attributes::a_halo_color>,
            InterpolationUniform<attributes::a_halo_width>,
            InterpolationUniform<attributes::a_halo_blur>,
            uniforms::u_opacity,
            uniforms::u_fill_color,
            uniforms::u_halo_color,
            uniforms::u_halo_width,
            uniforms::u_halo_blur
        >;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        AttributeBindings attributeBindings(const PossiblyEvaluated&) const;
        UniformValues uniformValues(float z, const PossiblyEvaluated&) const;
    };

    class PossiblyEvaluated {
    public:
        PossiblyEvaluatedPropertyValue<float> textOpacity;
        PossiblyEvaluatedPropertyValue<Color> textColor;
        PossiblyEvaluatedPropertyValue<Color> textHaloColor;
        PossiblyEvaluatedPropertyValue<float> textHaloWidth;
        PossiblyEvaluatedPropertyValue<float> textHaloBlur;
        std::array<float, 2> textTranslate;
        TranslateAnchorType textTranslateAnchor;

        Binders createBinders(float z) const;

        std::bitset<8> constants() const;
        std::vector<std::string> defines() const;
    };
};


} // namespace style
} // namespace mbgl
